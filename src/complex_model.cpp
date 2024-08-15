// ReSharper disable CppDFANullDereference
#include <iostream>
#include <print>

#define TINYOBJLOADER_IMPLEMENTATION
#include <complex_model.h>
#include <types.h>
namespace rendermesh {
    ComplexModel::ComplexModel(std::shared_ptr<MeshesBuffer>& buffer, const std::filesystem::path& path) :
        meshesBuffer(buffer) {

        std::fstream model(path);
        if (!model.is_open()) {
        }
        std::string warnings, errors;

        tinyobj::attrib_t attributes{};
        if (path.has_parent_path()) {
            mtlDir = path.parent_path();
        }
        LoadObj(&attributes, &shapes, &materials, &warnings, &errors, path.c_str(), mtlDir.c_str());
        if (!errors.empty()) {
            std::cout << errors << '\n';
            std::terminate();
        }

        MeshModel mesh{};
        for (const auto& shape : shapes) {
            mesh.pipeline = graphics++;

            std::string texName;
            buildTriangles(shape, mesh, attributes.vertices, attributes.normals, attributes.texcoords);

            if (mesh.textured < materials.size()) {
                texName = materials[mesh.textured].diffuse_texname;
                if (texName.empty())
                    texName = materials[mesh.textured].bump_texname;

                while (texName.find('\\') != std::string::npos) {
                    auto begin{texName.find('\\')};
                    auto end{begin};
                    while (texName[end] == '\\')
                        end++;

                    texName[begin] = '/';
                    texName.erase(begin + 1, end - begin - 1);
                }
            }

            getTexturePath(mesh.texture, path, texName);
            meshes.emplace_back(std::move(mesh));
        }
    }
    void ComplexModel::populateBuffers() const {
        for (const auto& mesh : meshes) {
            meshesBuffer->bindMeshModel(mesh.pipeline);

            meshesBuffer->loadTexture(mesh.texture);
            meshesBuffer->bindBuffer(mesh.triangles, mesh.indices);
        }
    }

    void ComplexModel::draw() const {
        for (const auto& mesh : meshes) {
            meshesBuffer->bindMeshModel(mesh.pipeline);
            meshesBuffer->drawBuffers(mesh.indices.size());
        }
    }

    void ComplexModel::buildTriangles(const tinyobj::shape_t& shape, MeshModel& modelMesh,
        const std::vector<f32>& vertices,
        const std::vector<f32>& normals, const std::vector<f32>& texcoords) {

        const auto& mesh{shape.mesh};
        auto& triangles{modelMesh.triangles};
        auto& indices{modelMesh.indices};

        modelMesh.textured = mesh.material_ids[0];

        for (const auto& realMap : mesh.indices) {
            Vertex vertex{};
            vertex.position = {
                vertices[3 * realMap.vertex_index + 0],
                vertices[3 * realMap.vertex_index + 1],
                vertices[3 * realMap.vertex_index + 2],
            };
            if (!normals.empty()) {
                vertex.normal = {
                    normals[3 * realMap.normal_index + 0],
                    normals[3 * realMap.normal_index + 1],
                    normals[3 * realMap.normal_index + 2],
                };
            }
            if (!texcoords.empty()) {
                const auto texCoordX{texcoords[2 * realMap.texcoord_index + 0]};
                const auto texCoordY{texcoords[2 * realMap.texcoord_index + 1]};
                if (texCoordX >= 0 && texCoordY >= 0) {
                    vertex.texture = {
                        texCoordX, 1.f - texCoordY
                    };
                }
            }

            if (!uniqueVertices.contains(vertex)) {
                uniqueVertices[vertex] = static_cast<u32>(triangles.size());
                triangles.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
        std::print("Total triangles obtained: {}\n", triangles.size());
        std::print("Total indices obtained: {}\n", indices.size());
    }

    void ComplexModel::getTexturePath(std::filesystem::path& output,
        const std::filesystem::path& model, const std::filesystem::path& tex) const {

        auto imagePath{model};

        if (!tex.has_filename()) {
            imagePath.replace_extension("jpg");
            if (exists(imagePath))
                output = imagePath;
            imagePath.replace_extension("png");
            if (exists(imagePath))
                output = imagePath;
        } else {
            output = mtlDir / tex;
        }
        std::print("Texture path {}\n", output.string());
        if (!exists(output) && !is_regular_file(output))
            throw std::runtime_error("No texture was loaded for the object, check the import locations");
    }
}
