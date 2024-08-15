#pragma once
#include <tiny_obj_loader.h>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <mesh_buffer.h>
#include <camera.h>
namespace rendermesh {

    class ComplexMesh {
    public:
        explicit ComplexMesh(std::shared_ptr<MeshBuffer>& buffer, const std::filesystem::path& path);

        void populateBuffers() const;
        void draw() const;
    private:
        std::shared_ptr<MeshBuffer> meshBuffer;

        std::vector<Vertex> triangles{};
        std::vector<GLuint> indices{};

        std::vector<tinyobj::shape_t> shapes{};
        std::vector<tinyobj::material_t> materials{};
        void loadAllVertices(const std::vector<tinyobj::shape_t>& shapes, const std::vector<f32>& vertices, const std::vector<f32>& normals, const std::vector<f32>& texcoords);

        u64 textured{std::numeric_limits<u64>::max()};
        std::pair<std::string, std::filesystem::path> texture;
    };
}
