// ReSharper disable CppDFANullDereference
#include <iostream>
#include <fstream>
#include <print>
#include <span>

#include <assimp/postprocess.h>

#include <mesh/complex.h>
namespace rendermesh::mesh {
    Complex::Complex(const std::shared_ptr<buffer::Submit>& submit,
        const std::filesystem::path& path) :
        buffers(submit) {

        std::fstream model(path);
        if (path.has_parent_path()) {
            objDir = path.parent_path();
        }
        Assimp::Importer importer;
        const auto scene{importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs)};
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error(std::format("Assimp: {}", importer.GetErrorString()));
        }

        u32 parent{};
        u32 depth{};
        std::function<void(const aiNode&, const aiMatrix4x4&)> processAiNode =
                [&](const aiNode& node, const aiMatrix4x4& transform) {
            const auto nodeTransform{transform * node.mTransformation};
            if (!node.mMeshes) {
                parent++;
            }
            NodeTransform space{parent, depth, nodeTransform};

            for (const auto aiMesh : std::span(node.mMeshes, node.mNumMeshes)) {
                const auto implMesh{scene->mMeshes[aiMesh]};
                space.domain = Local;

                Model moldable{static_cast<u32>(transforms.size()), buffers->buffers.generate()};
                processMesh(moldable, *implMesh, *scene);
                meshes.emplace_back(std::move(moldable));
            }
            transforms.emplace_back(space);

            for (const auto children : std::span(node.mChildren, node.mNumChildren)) {
                depth++;
                processAiNode(*children, nodeTransform);
            }
        };
        processAiNode(*scene->mRootNode, aiMatrix4x4());
    }
    void Complex::populateBuffers() const {
        struct ReusableTextures {
            std::filesystem::path path;
            u32 pipe;
        };
        std::vector<ReusableTextures> current;
        for (const auto& mesh : meshes) {
            buffers->bindMeshModel(mesh.pipeline);
            for (const auto& textures : mesh.textures) {
                const std::filesystem::path& path{textures.path};

                ReusableTextures reusing{};
                for (const auto& loaded : current) {
                    if (loaded.path == path) {
                        reusing = loaded;
                    }
                }
                if (!is_empty(reusing.path)) {
                    std::print("The texture {} has already been loaded previously\n", path.string());
                    buffers->reuseTexture(mesh.pipeline, reusing.pipe);
                    buffers->mixTexture(mesh.identifier, textures);
                } else {
                    buffers->loadTexture(mesh.identifier, textures);
                    current.emplace_back(path, mesh.pipeline);
                }

                buffers->bindVertexObjects(mesh.triangles, mesh.indices);
            }
        }
    }

    void Complex::draw() const {
        for (const auto& mesh : meshes) {
            glm::mat4 nodeMatrix{1.0f};
            buffers->bindMeshModel(mesh.pipeline);
            // Due to the way the GLTF format is structured, we must multiply the matrices of child nodes
            // by that of the highest-level parent
            const auto position{transforms[mesh.level].position};
            const auto scaleEye{transforms[mesh.level].scale};

            i64 level{mesh.level};
            for (; level >= 0; level--) {
                while (transforms[level].domain != Global)
                    level--;
                nodeMatrix *= toMat4(transforms[level].rotation);
            }
            nodeMatrix *= toMat4(transforms[mesh.level].rotation);
            nodeMatrix = translate(nodeMatrix, position);

            nodeMatrix = scale(nodeMatrix, scaleEye);
            buffers->accMvp(nodeMatrix);
            buffers->drawObjects(mesh.identifier, mesh.indices.size());
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
            throw std::runtime_error("File system failure");

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
