#pragma once
#include <filesystem>
#include <array>

#include <glm/glm.hpp>
#include <tiny_obj_loader.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <shaders.h>
#include <types.h>
namespace rendermesh {
    enum BoundsLayout {
        PositionVBO,
        EBO
    };
    struct Vertex {
        Vertex() = default;
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texture;

        auto operator == (const Vertex& other) const {
            return other.position == position && other.normal == normal && other.texture == texture;
        }
    };
    struct HashVertex {
        u64 operator()(const Vertex& other) const {
            return std::hash<float>()(other.position.x) ^ std::hash<float>()(other.position.y) ^
                std::hash<float>()(other.position.z);
        }
    };

    struct MeshesBuffer {
        const u32 positionsAttr{0};
        const u32 normalsAttr{1};
        const u32 texturesAttr{2};

        MeshesBuffer() = default;

        void reset() const;
        void bind() const;
        void createVAO();

        void bindBuffer(const std::vector<Vertex>& triangles, const std::vector<GLuint>& indices) const;
        void draw(GLsizei indices) const;

        GLuint vao{};
        std::array<GLuint, EBO + 1> vbos;
    };

    class ComplexMesh {
    public:
        explicit ComplexMesh(Shaders& shader, const std::filesystem::path& path);

        void populateBuffers() const;
        void draw() const;
    private:
        Shaders& attach;
        MeshesBuffer meshes;

        std::vector<Vertex> triangles;
        std::vector<GLuint> indices;

        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        void loadAllVertices(const std::vector<tinyobj::shape_t>& shapes, const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float> & texcoords);
    };
}
