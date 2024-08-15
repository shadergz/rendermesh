// ReSharper disable CppDFANullDereference
#include <iostream>
#include <fstream>
#include <print>
#include <span>

#include <assimp/postprocess.h>
#include <complex_model.h>
namespace rendermesh {
    ComplexModel::ComplexModel(const std::shared_ptr<MeshesBuffer>& buffer, const std::filesystem::path& path) :
        meshesBuffer(buffer) {

        std::fstream model(path);
        if (!model.is_open()) {
        }
        if (path.has_parent_path()) {
            objDir = path.parent_path();
        }
        Assimp::Importer importer;
        const auto scene{importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs)};
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error(std::format("Assimp: {}", importer.GetErrorString()));
        }

        MeshModel mesh{};

        std::function<void(const aiNode& node)> processAiNode = [&](const aiNode& node) {
            for (const auto aiMesh : std::span(node.mMeshes, node.mNumMeshes)) {
                const auto implMesh{scene->mMeshes[aiMesh]};
                processMesh(mesh, *implMesh, *scene);
                mesh.pipeline = graphics++;
                meshes.emplace_back(std::move(mesh));

                mesh = {};
            }
            for (const auto children : std::span(node.mChildren, node.mNumChildren)) {
                processAiNode(*children);
            }
        };
        processAiNode(*scene->mRootNode);
    }
    void ComplexModel::populateBuffers() const {
        for (const auto& mesh : meshes) {
            meshesBuffer->bindMeshModel(mesh.pipeline);

            for (const auto& textures : mesh.textures) {
                meshesBuffer->loadTexture(textures.path);
            }
            meshesBuffer->bindBuffer(mesh.triangles, mesh.indices);
        }
    }

    void ComplexModel::draw() const {
        for (const auto& mesh : meshes) {
            meshesBuffer->bindMeshModel(mesh.pipeline);
            meshesBuffer->drawBuffers(mesh.indices.size());
        }
    }

    void ComplexModel::processMesh(MeshModel& modelMesh,
        const aiMesh& implMesh, const aiScene& scene) const {

        std::unordered_map<Vertex, u32, HashVertex> uniqueVertices;
        auto& triangles{modelMesh.triangles};
        auto& indices{modelMesh.indices};

        const auto vertices{std::span(implMesh.mVertices, implMesh.mNumVertices)};

        std::span<aiVector3D> normals{};
        if (implMesh.mNormals) {
            normals = std::span(implMesh.mNormals, implMesh.mNumVertices);
        }
        std::span<aiVector3D> texcoords{};
        if (implMesh.mTextureCoords[0]) {
            texcoords = std::span(implMesh.mTextureCoords[0], implMesh.mNumVertices);
        }

        for (u32 verticesCount{}; verticesCount < implMesh.mNumVertices; verticesCount++) {

            Vertex vertex{};
            vertex.position = {
                vertices[verticesCount].x,
                vertices[verticesCount].y,
                vertices[verticesCount].z
            };
            if (!normals.empty()) {
                vertex.normal = {
                    normals[verticesCount].x,
                    normals[verticesCount].y,
                    normals[verticesCount].z,
                };
            }
            if (texcoords.size() > verticesCount) {
                vertex.texture = {
                    texcoords[verticesCount].x,
                    texcoords[verticesCount].y,
                };
            }
            triangles.push_back(vertex);
        }

        for (const auto face : std::span(implMesh.mFaces, implMesh.mNumFaces)) {
            for (const auto index : std::span(face.mIndices, face.mNumIndices)) {
                indices.push_back(index);
            }
        }

        if (implMesh.mMaterialIndex != -1) {
            auto& textures{modelMesh.textures};

            const auto material{scene.mMaterials[implMesh.mMaterialIndex]};
            auto diffuseMaps{getTextureMaps(*material, aiTextureType_DIFFUSE)};
            auto specularMaps{getTextureMaps(*material, aiTextureType_SPECULAR)};
            auto emissiveMaps{getTextureMaps(*material, aiTextureType_EMISSIVE)};

            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
        }

        std::print("Total triangles obtained: {}\n", triangles.size());
        std::print("Total indices obtained: {}\n", indices.size());
    }

    std::vector<Texture> ComplexModel::getTextureMaps(
        const aiMaterial& material, const aiTextureType type) const {
        std::vector<Texture> textures;

        if (objDir.empty())
            throw std::runtime_error("");

        Texture meshMaterial;
        for (u32 matIndex{}; matIndex < material.GetTextureCount(type); matIndex++) {
            aiString aiPath;
            material.GetTexture(type, matIndex, &aiPath);

            std::filesystem::path texture(objDir);
            texture.append(std::string(aiPath.data, std::strlen(aiPath.data)));

            std::print("Texture path {}\n", texture.string());
            if (!exists(texture) && !is_regular_file(texture))
                throw std::runtime_error("No texture was loaded for the object, check the import locations");

            meshMaterial.path = texture;
            meshMaterial.type = type;

            textures.emplace_back(meshMaterial);
        }

        return textures;
    }
}
