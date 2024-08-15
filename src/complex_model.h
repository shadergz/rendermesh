#pragma once
#include <tiny_obj_loader.h>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <meshes_buffer.h>
#include <camera.h>
namespace rendermesh {

    struct MeshModel {
        MeshModel() = default;
        std::vector<Vertex> triangles;
        std::vector<GLuint> indices;

        u32 pipeline;
        u64 textured{std::numeric_limits<u64>::max()};

        std::pair<std::string, std::filesystem::path> texture;
        bool isEnb{};
    };

    class ComplexModel {
    public:
        explicit ComplexModel(std::shared_ptr<MeshesBuffer>& buffer, const std::filesystem::path& path);

        void populateBuffers() const;
        void draw() const;
    private:
        std::shared_ptr<MeshesBuffer> meshesBuffer;
        std::vector<MeshModel> meshes;

        std::vector<tinyobj::shape_t> shapes{};
        std::vector<tinyobj::material_t> materials{};

        static void buildTriangles(const tinyobj::shape_t& shape, MeshModel& modelMesh,
            const std::vector<f32>& vertices,
            const std::vector<f32>& normals,
            const std::vector<f32>& texcoords);

        void getTextureName(std::pair<std::string, std::filesystem::path>& text,
            const std::filesystem::path& path, std::string& name);

        std::filesystem::path mtlDir{"./"};
    };
}
