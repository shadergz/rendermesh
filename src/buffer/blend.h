#pragma once
#include <memory>
#include <assimp/material.h>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <raster/shaders.h>
namespace rendermesh::buffer {
    struct MixableMaterial {
        GLuint texture{};
        aiTextureType type{};
        bool inUse{};

        void set(GLuint tex);
    };

    class Blend {
    public:
        Blend() = default;
        explicit Blend(aiTextureType type, GLuint texture);

        MixableMaterial diffuse{.type = aiTextureType_DIFFUSE};
        MixableMaterial specular{.type = aiTextureType_SPECULAR};
        MixableMaterial emissive{.type = aiTextureType_EMISSIVE};

        void bind(const std::shared_ptr<raster::Shaders>& shader, aiTextureType type) const;
        void blending(aiTextureType type, GLuint texture);
    };
}