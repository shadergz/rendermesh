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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * triangles.size(), &triangles[0], GL_STATIC_DRAW);

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
        std::string errors;

        tinyobj::attrib_t attributes;
        LoadObj(&attributes, &shapes, &materials, &errors, path.c_str(), "./");
        if (!errors.empty()) {
            std::cout << errors << '\n';
            std::terminate();
        }

        u32 starts{};
        loadAllVertices(shapes, starts, attributes.vertices, attributes.normals, attributes.texcoords);
    }
    void ComplexMesh::populateBuffers() const {
        meshes.bindBuffer(triangles, indices);
    }
    void ComplexMesh::draw() const {
        attach.useShaders();
        meshes.draw(indices.size());
    }

    void ComplexMesh::loadAllVertices(const std::vector<tinyobj::shape_t>& shapes, u32& starts, const std::vector<float>& vertices,
        const std::vector<float>& normals, const std::vector<float>& texcoords) {

        for (const auto& shape : shapes) {
            const auto& mesh{shape.mesh};

            const u64 filledSize{triangles.size()};
            triangles.resize(filledSize + mesh.indices.size());

            u64 mei{};
            for (; mei < mesh.indices.size(); mei++) {
                const auto& properties{mesh.indices[mei]};
                triangles[mei + starts].position = {
                    vertices[properties.vertex_index * 3],
                    vertices[properties.vertex_index * 3 + 1],
                    vertices[properties.vertex_index * 3 + 2]
                };

                triangles[mei + starts].normal = {
                    normals[properties.normal_index * 3],
                    normals[properties.normal_index * 3 + 1],
                    normals[properties.normal_index * 3 + 2]
                };
                triangles[mei + starts].texture = {};
                if (properties.texcoord_index >= 0) {
                    triangles[mei + starts].texture = {
                        texcoords[properties.texcoord_index * 2],
                        texcoords[properties.texcoord_index * 2 + 1],
                    };
                }
                indices.emplace_back(mei + starts);
            }
            starts += mei;
        }
    }
}
