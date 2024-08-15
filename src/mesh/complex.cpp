// ReSharper disable CppDFANullDereference
#include <iostream>
#include <fstream>
#include <print>
#include <span>

#include <assimp/postprocess.h>

#include <mesh/complex.h>
namespace rendermesh::mesh {
    Complex::Complex(const std::shared_ptr<buffer::Submit>& submit, const std::filesystem::path& path) :
        submitter(submit) {

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

        Model mesh{};
        u32& parent{mesh.parent};
        u32& depth{mesh.depth};

        aiVector3D scaling,
            translation;
        aiQuaternion rotation;

        std::function<void(const aiNode&, aiMatrix4x4&)> processAiNode =
                [&](const aiNode& node, const aiMatrix4x4& transform) {
            auto nodeTransform{transform * node.mTransformation};

            for (const auto aiMesh : std::span(node.mMeshes, node.mNumMeshes)) {
                const auto implMesh{scene->mMeshes[aiMesh]};
                depth++;
                processMesh(mesh, *implMesh, *scene);
                nodeTransform.Decompose(scaling, rotation, translation);

                mesh.scale = glm::vec3(scaling.x, scaling.y, scaling.z);
                mesh.position = glm::vec3(translation.x, translation.y, translation.z);
                mesh.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);

                mesh.pipeline = submitter->buffers.generate();
                meshes.emplace_back(std::move(mesh));
            }

            for (const auto children : std::span(node.mChildren, node.mNumChildren)) {
                depth = {};
                if (children->mMeshes)
                    parent++;
                processAiNode(*children, nodeTransform);
            }
        };
        aiMatrix4x4 transform{};
        processAiNode(*scene->mRootNode, transform);
    }
    void Complex::populateBuffers() const {
        struct ReusableTextures {
            std::filesystem::path path;
            u32 pipe;
        };
        std::vector<ReusableTextures> current;
        for (const auto& mesh : meshes) {
            submitter->bindMeshModel(mesh.pipeline);

            for (const auto& textures : mesh.textures) {
                const std::filesystem::path& path{textures.path};

                ReusableTextures reusing{};
                for (const auto& loaded : current) {
                    if (loaded.path == path) {
                        reusing = loaded;
                    }
                }
                if (reusing.path.empty()) {
                    submitter->loadTexture(mesh.identifier, textures);
                    current.emplace_back(path, mesh.pipeline);
                } else {
                    std::print("The texture {} has already been loaded previously\n", path.string());
                    submitter->buffers.reuse(mesh.pipeline, reusing.pipe);
                    submitter->mixTexture(mesh.identifier, textures);
                }

                submitter->bindBuffer(mesh.triangles, mesh.indices);
            }
        }
    }

    void Complex::draw() const {
        static std::unordered_map<u32, glm::mat4> parents;
        for (const auto& mesh : meshes) {
            glm::mat4 nodeMatrix{1.0f};

            if (!parents.contains(mesh.parent)) {
                parents[mesh.parent] = toMat4(mesh.rotation);
            }

            submitter->bindMeshModel(mesh.pipeline);
            // Due to the way the GLTF format is structured, we must multiply the matrices of child nodes
            // by that of the highest-level parent
            nodeMatrix = translate(nodeMatrix, mesh.position);
            nodeMatrix *= toMat4(mesh.rotation);

            if (mesh.parent) {
                i64 upward{mesh.parent};
                while (upward >= 0) {
                    if (parents.contains(upward))
                        nodeMatrix *= parents[upward--];
                    else
                        upward--;
                }
            } else {
                nodeMatrix *= parents[0];
            }
            nodeMatrix = scale(nodeMatrix, mesh.scale);
            submitter->accMvp(nodeMatrix);
            submitter->drawBuffers(mesh.identifier, mesh.indices.size());
        }
    }

    void Complex::processMesh(Model& modelMesh,
        const aiMesh& implMesh, const aiScene& scene) const {

        std::unordered_map<buffer::Vertex, u32, buffer::HashVertex> uniqueVertices;
        auto& triangles{modelMesh.triangles};
        auto& indices{modelMesh.indices};

        modelMesh.identifier = std::hash<std::string>()(std::string(implMesh.mName.data, std::strlen(implMesh.mName.data)));

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

            buffer::Vertex vertex{};
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

    std::vector<buffer::Texture> Complex::getTextureMaps(
        const aiMaterial& material, const aiTextureType type) const {
        std::vector<buffer::Texture> textures;

        if (objDir.empty())
            throw std::runtime_error("");

        buffer::Texture meshMaterial;
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
