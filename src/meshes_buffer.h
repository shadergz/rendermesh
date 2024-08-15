#pragma once

#include <filesystem>
#include <list>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <types.h>
#include <shaders.h>
namespace rendermesh {
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

    struct MeshPipelineBuffers {
        MeshPipelineBuffers() {

        }
        GLuint texture{};
        GLuint ebo{};
        GLuint vbo{};
    };

    class MeshesBuffer {
    public:
        const u32 positionsAttr{0};
        const u32 normalsAttr{1};
        const u32 texturesAttr{2};

        explicit MeshesBuffer(std::list<MeshPipelineBuffers>& glBuffers)
            : pipelines(glBuffers) {
        }
        auto size() const {
            return pipelines.size();
        }

        void drawBuffers(u32 indices) const;
        void bindMeshModel(u32 model);

        void bindBuffer(const std::vector<Vertex>& triangles, const std::vector<GLuint>& indices) const;

        void loadTexture(const std::filesystem::path& path);
    private:
        std::list<MeshPipelineBuffers>& pipelines;

        GLuint texture{};
        GLuint ebo{};
        GLuint vbo{};
    };
}
