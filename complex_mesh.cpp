// ReSharper disable CppDFANullDereference
#include <fstream>
#include <iostream>
#include <print>

#define TINYOBJLOADER_IMPLEMENTATION
#include <complex_mesh.h>
#include <types.h>
namespace rendermesh {
    ComplexMesh::ComplexMesh(std::shared_ptr<MeshBuffer>& buffer, const std::filesystem::path& path) :
        meshBuffer(buffer) {

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
        auto imagePath{path};

        imagePath.replace_extension("jpg");

        if (exists(imagePath))
            texture = std::make_pair(imagePath.string(), std::filesystem::path{});

        imagePath.replace_extension("png");
        if (exists(imagePath))
            texture = std::make_pair(imagePath.string(), std::filesystem::path{});
    }
    void ComplexMesh::populateBuffers() const {
        meshBuffer->loadTexture({std::get<1>(texture) / std::get<0>(texture)});

        meshBuffer->bindBuffer(triangles, indices);
    }

    void ComplexMesh::draw() const {
        meshBuffer->drawBuffers(indices.size());
    }

    void ComplexMesh::loadAllVertices(const std::vector<tinyobj::shape_t>& shapes, const std::vector<f32>& vertices,
        const std::vector<f32>& normals, const std::vector<f32>& texcoords) {

        std::unordered_map<Vertex, u32, HashVertex> uniqueVertices;
        const auto& model{shapes[0]};
        const auto& mesh{model.mesh};
        textured = mesh.material_ids[0];

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
}
