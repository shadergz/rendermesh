#pragma once
#include <tiny_obj_loader.h>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <meshes_buffer.h>
#include <shaders.h>
namespace rendermesh {

    class ComplexMesh {
    public:
        explicit ComplexMesh(Shaders& shader, const std::filesystem::path& path);

        void populateBuffers();
        void draw() const;
    private:
        Shaders& attach;
        MeshesBuffer meshes{};

        std::vector<Vertex> triangles{};
        std::vector<GLuint> indices{};

        std::vector<tinyobj::shape_t> shapes{};
        std::vector<tinyobj::material_t> materials{};
        void loadAllVertices(const std::vector<tinyobj::shape_t>& shapes, const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& texcoords);

        u64 textured{std::numeric_limits<u64>::max()};
        std::pair<std::string, std::filesystem::path> texture;
    };
}
