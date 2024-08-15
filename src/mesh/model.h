#pragma once

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <view/camera.h>
#include <buffer/vertex.h>
#include <buffer/texture.h>
namespace rendermesh::mesh {
    struct Model {
        Model() = default;
        std::vector<buffer::Vertex> triangles;
        std::vector<GLuint> indices;

        u32 pipeline;
        std::vector<buffer::Texture> textures;
        u64 identifier;

        glm::vec3 scale;
        glm::vec3 position;
        glm::quat rotation;

        u32 parent{};
        u32 depth{};
    };
}
