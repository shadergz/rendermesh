#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <meshes_buffer.h>
#include <camera.h>
namespace rendermesh {
    struct Texture {
        std::filesystem::path path;
        aiTextureType type;
    };

    struct MeshModel {
        MeshModel() = default;
        std::vector<Vertex> triangles;
        std::vector<GLuint> indices;

        u32 pipeline;
        std::vector<Texture> textures;
    };

    class ComplexModel {
    public:
        explicit ComplexModel(const std::shared_ptr<MeshesBuffer>& buffer, const std::filesystem::path& path);

        void populateBuffers() const;
        void draw() const;
    private:
        std::shared_ptr<MeshesBuffer> meshesBuffer;
        std::vector<MeshModel> meshes;

        void processMesh(MeshModel& modelMesh, const aiMesh& implMesh, const aiScene& scene) const;
        std::vector<Texture> getTextureMaps(const aiMaterial& material, aiTextureType type) const;

        u32 graphics{};
        std::filesystem::path objDir;
    };
}
