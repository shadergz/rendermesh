// ReSharper disable CppDFANullDereference
#include <fstream>
#include <iostream>
#include <print>

#define TINYOBJLOADER_IMPLEMENTATION
#include <complex_mesh.h>
#include <types.h>
namespace rendermesh {
    ComplexMesh::ComplexMesh(Shaders& shader,
            const std::filesystem::path& path) : attach(shader) {
        meshes.createVAO();

        std::fstream model(path);
        if (!model.is_open()) {
        }
        std::string warnings, errors;

        tinyobj::attrib_t attributes{};
        LoadObj(&attributes, &shapes, &materials, &warnings, &errors, path.c_str(), "./");
        if (!errors.empty()) {
            std::cout << errors << '\n';
            std::terminate();
        }

        loadAllVertices(shapes, attributes.vertices, attributes.normals, attributes.texcoords);
    }
    void ComplexMesh::populateBuffers() {
        meshes.bind();
        meshes.bindBuffer(triangles, indices);
        meshes.loadTexture("");
    }
    void ComplexMesh::draw() const  {
        attach.useShaders();
        meshes.draw(indices.size());
    }

    void ComplexMesh::loadAllVertices(const std::vector<tinyobj::shape_t>& shapes, const std::vector<float>& vertices,
        const std::vector<float>& normals, const std::vector<float>& texcoords) {

        std::unordered_map<Vertex, u32, HashVertex> uniqueVertices;
        u32 total{};
        for (const auto& shape : shapes) {
            const auto& mesh{shape.mesh};
            total += mesh.indices.size();

            u64 index{};
            for (; index < mesh.indices.size(); index++) {
                Vertex piramide{};
                const auto& realMap{mesh.indices[index]};
                piramide.position = {
                    vertices[3 * realMap.vertex_index + 0],
                    vertices[3 * realMap.vertex_index + 1],
                    vertices[3 * realMap.vertex_index + 2],
                };
                piramide.normal = {
                    normals[3 * realMap.normal_index + 0],
                    normals[3 * realMap.normal_index + 1],
                    normals[3 * realMap.normal_index + 2],
                };

                piramide.texture = {};

                if (!texcoords.empty()) {
                    const auto textCoord0{texcoords[3 * realMap.texcoord_index + 0]};
                    const auto textCoord1{texcoords[3 * realMap.texcoord_index + 1]};
                    if (textCoord0 >= 0 && textCoord1 >= 0) {
                        piramide.texture = {
                            textCoord0, textCoord1
                        };
                    }
                }

                if (!uniqueVertices.contains(piramide)) {
                    uniqueVertices[piramide] = triangles.size();
                    triangles.push_back(piramide);
                }
                indices.push_back(uniqueVertices[piramide]);
            }
        }
        std::print("Total triangles obtained: {}\n", triangles.size());
        std::print("Total indices obtained: {}\n", indices.size());
    }
}
