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
    enum TransformLevel {
        Global,
        Local
    };

    struct NodeTransform {
        explicit NodeTransform(u32 parent, u32 depth, const aiMatrix4x4& matrix = aiMatrix4x4());

        glm::vec3 scale{};
        glm::vec3 position{};
        glm::quat rotation{};

        u32 parent;
        u32 depth;
        TransformLevel domain{Global};
    };
    struct Model {
        explicit Model(const u32 in, const u32 pipe = 0) :
            pipeline(pipe), level(in) {}

        std::vector<buffer::Vertex> triangles{};
        std::vector<GLuint> indices{};

        u32 pipeline{};
        std::vector<buffer::Texture> textures{};
        u64 identifier{};

        u32 level;
    };
}
