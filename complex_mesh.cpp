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
        std::filesystem::path mtlDir{"./"};
        if (path.has_parent_path()) {
            mtlDir = path.parent_path();
        }
        LoadObj(&attributes, &shapes, &materials, &warnings, &errors, path.c_str(), mtlDir.c_str());
        if (!errors.empty()) {
            std::cout << errors << '\n';
            std::terminate();
        }

        loadAllVertices(shapes, attributes.vertices, attributes.normals, attributes.texcoords);

        std::string texName{};
        if (textured < materials.size())
            texName = materials[textured].diffuse_texname;
        if (!texName.empty())
            texture = std::make_pair(texName, mtlDir);
        auto objTextured{path};

        objTextured.replace_extension("jpg");

        if (exists(objTextured))
            texture = std::make_pair(objTextured.string(), std::filesystem::path{});

        objTextured.replace_extension("png");
        if (exists(objTextured))
            texture = std::make_pair(objTextured.string(), std::filesystem::path{});
    }
    void ComplexMesh::populateBuffers() {
        meshes.bind();
        meshes.loadTexture({std::get<1>(texture) / std::get<0>(texture)});

        meshes.bindBuffer(triangles, indices);
    }
    void ComplexMesh::draw() const {
        attach.useShaders();
        meshes.draw(indices.size());
    }

    void ComplexMesh::loadAllVertices(const std::vector<tinyobj::shape_t>& shapes, const std::vector<float>& vertices,
        const std::vector<float>& normals, const std::vector<float>& texcoords) {

        std::unordered_map<Vertex, u32, HashVertex> uniqueVertices;
        const auto& model{shapes[0]};
        const auto& mesh{model.mesh};
        textured = mesh.material_ids[0];

        for (u64 index{}; index < mesh.indices.size(); index++) {
            Vertex piramide{};
            const auto& realMap{mesh.indices[index]};
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
        std::print("Total triangles obtained: {}\n", triangles.size());
        std::print("Total indices obtained: {}\n", indices.size());
    }
}
