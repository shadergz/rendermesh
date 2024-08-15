// ReSharper disable CppDFANullDereference
#include <fstream>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <complex_mesh.h>
#include <types.h>
namespace rendermesh {
    void MeshesBuffer::reset() const {
        if (vao) {
        }
    }
    void MeshesBuffer::bind() const {
        glBindVertexArray(vao);
    }
    void MeshesBuffer::createVAO() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(2, &vbos[0]);
    }

    void MeshesBuffer::bindBuffer(const std::vector<Vertex>& triangles, const std::vector<GLuint>& indices) const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[PositionVBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangles[0]) * triangles.size(), &triangles[0], GL_STATIC_DRAW);

        // We are using the AOS memory model
        // ReSharper disable once CppRedundantCastExpression
        glVertexAttribPointer(positionsAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(positionsAttr);

        // ReSharper disable once CppRedundantCastExpression
        glVertexAttribPointer(normalsAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(normalsAttr);

        // ReSharper disable once CppRedundantCastExpression
        glVertexAttribPointer(texturesAttr, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texture)));
        glEnableVertexAttribArray(texturesAttr);

        glBindVertexArray(0);
    }

    void MeshesBuffer::draw(const GLsizei indices) const {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, nullptr);
    }

    ComplexMesh::ComplexMesh(Shaders& shader,
            const std::filesystem::path& path) : attach(shader) {
        meshes.reset();
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
    void ComplexMesh::populateBuffers() const {
        meshes.bind();
        meshes.bindBuffer(triangles, indices);
    }
    void ComplexMesh::draw() const  {
        attach.useShaders();
        meshes.draw(indices.size());
    }

    void ComplexMesh::loadAllVertices(const std::vector<tinyobj::shape_t>& shapes, const std::vector<float>& vertices,
        const std::vector<float>& normals, const std::vector<float>& texcoords) {

        std::unordered_map<Vertex, u32, HashVertex> uniqueVertices;
        for (const auto& shape : shapes) {
            const auto& mesh{shape.mesh};

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
    }
}
