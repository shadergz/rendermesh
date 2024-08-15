// ReSharper disable CppDFANullDereference
#include <fstream>
#include <iostream>
#include <print>

#define TINYOBJLOADER_IMPLEMENTATION
#include <complex_model.h>
#include <types.h>
namespace rendermesh {
    ComplexModel::ComplexModel(std::shared_ptr<MeshesBuffer>& buffer, const std::filesystem::path& path) :
        meshesBuffer(buffer) {

        meshes.resize(meshesBuffer->size());

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

        u32 shapeIndex{};

        for (auto& modelMesh : meshes) {
            if (shapes.size() < shapeIndex)
                continue;

            auto& currShape{shapes[shapeIndex]};
            modelMesh.pipeline = shapeIndex++;

            std::string texName;
            buildTriangles(currShape, modelMesh, attributes.vertices, attributes.normals, attributes.texcoords);

            if (modelMesh.textured < materials.size())
                texName = materials[modelMesh.textured].diffuse_texname;

            getTextureName(modelMesh.texture, path, texName);
            modelMesh.isEnb = true;
        }
    }
    void ComplexModel::populateBuffers() const {
        std::for_each(meshes.begin(), meshes.end(), [&](const auto& mesh) {
            if (!mesh.isEnb)
                return false;

            meshesBuffer->bindMeshModel(mesh.pipeline);

            meshesBuffer->loadTexture({std::get<1>(mesh.texture) / std::get<0>(mesh.texture)});
            meshesBuffer->bindBuffer(mesh.triangles, mesh.indices);
            return true;
        });
    }

    void ComplexModel::draw() const {
        std::for_each(meshes.begin(), meshes.end(), [&](auto& mesh) {
            if (!mesh.isEnb)
                return false;

            meshesBuffer->bindMeshModel(mesh.pipeline);
            meshesBuffer->drawBuffers(mesh.indices.size());
            return true;
        });
    }

    void ComplexModel::buildTriangles(
        const tinyobj::shape_t& shape,
        MeshModel& modelMesh,
        const std::vector<f32>& vertices,
        const std::vector<f32>& normals, const std::vector<f32>& texcoords) {

        std::unordered_map<Vertex, u32, HashVertex> uniqueVertices;
        const auto& mesh{shape.mesh};
        auto& triangles{modelMesh.triangles};
        auto& indices{modelMesh.indices};

        modelMesh.textured = mesh.material_ids[0];

        for (const auto& realMap : mesh.indices) {
            Vertex piramide{};
            piramide.position = {
                vertices[3 * realMap.vertex_index + 0],
                vertices[3 * realMap.vertex_index + 1],
                vertices[3 * realMap.vertex_index + 2],
            };
            if (!normals.empty()) {
                piramide.normal = {
                    normals[3 * realMap.normal_index + 0],
                    normals[3 * realMap.normal_index + 1],
                    normals[3 * realMap.normal_index + 2],
                };
            }
            if (!texcoords.empty()) {
                const auto texCoordX{texcoords[2 * realMap.texcoord_index + 0]};
                const auto texCoordY{texcoords[2 * realMap.texcoord_index + 1]};
                if (texCoordX >= 0 && texCoordY >= 0) {
                    piramide.texture = {
                        texCoordX, 1.f - texCoordY
                    };
                }
            }

            if (!uniqueVertices.contains(piramide)) {
                uniqueVertices[piramide] = static_cast<u32>(triangles.size());
                triangles.push_back(piramide);
            }
            indices.push_back(uniqueVertices[piramide]);
        }
        std::print("Total triangles obtained: {}\n", triangles.size());
        std::print("Total indices obtained: {}\n", indices.size());
    }

    void ComplexModel::getTextureName(
        std::pair<std::string, std::filesystem::path>& text,
        const std::filesystem::path& path,
        std::string& name) {

        auto imagePath{path};

        if (name.empty()) {
            imagePath.replace_extension("jpg");
            if (exists(imagePath))
                text = std::make_pair(imagePath.string(), std::filesystem::path{});
            imagePath.replace_extension("png");
            if (exists(imagePath))
                text = std::make_pair(imagePath.string(), std::filesystem::path{});
        } else {
            text = std::make_pair(name, mtlDir);
        }
        if (std::get<0>(text).empty()) {
            throw std::runtime_error("No texture was loaded for the object, check the import locations");
        }
    }
}
