#pragma once

#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <types.h>
#include <shaders.h>
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
        std::size_t operator()(const Vertex& other) const {
            return (std::hash<glm::vec3>()(other.position) ^
                (std::hash<glm::vec3>()(other.normal) ^
                    (std::hash<glm::vec2>()(other.texture) << 1)) >> 1);
        }
    };

    class MeshBuffer {
    public:
        const u32 positionsAttr{0};
        const u32 normalsAttr{1};
        const u32 texturesAttr{2};

        explicit MeshBuffer(std::array<GLuint, 2>& glBuffers)
            : vbos(glBuffers) {
        }
        void drawBuffers(u32 indices) const;

        void bindBuffer(const std::vector<Vertex>& triangles, const std::vector<GLuint>& indices) const;
        void loadTexture(const std::filesystem::path& path);
    private:
        GLuint texture{};

        std::array<GLuint, 2>& vbos;
    };
}
