#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace rendermesh::buffer {
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
                (std::hash<glm::vec3>()(other.normal) ^ (std::hash<glm::vec2>()(other.texture))));
        }
    };
}