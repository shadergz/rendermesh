#pragma once
#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <raster/shaders.h>
#include <buffer/pipeline.h>
#include <buffer/vertex.h>
#include <buffer/texture.h>
#include <buffer/blend.h>
namespace rendermesh::buffer {
    class Submit {
    public:
        const u32 positionsAttr{0};
        const u32 normalsAttr{1};
        const u32 texturesAttr{2};

        explicit Submit(const std::shared_ptr<raster::Shaders>& shaders, Pipeline& pipeline)
            : buffers(pipeline), shader(shaders) {}

        void bindMeshModel(u32 model);
        void bindBuffer(const std::vector<Vertex>& triangles, const std::vector<GLuint>& indices) const;
        void drawBuffers(u64 meshHash, u32 indices);

        void loadTexture(u64 meshHash, const Texture& resource);
        void mixTexture(u64 meshHash, const Texture& resource);

        void accMvp(const glm::mat4& point, bool isView = false);
        void reset();

        Pipeline& buffers;
    private:
        std::shared_ptr<raster::Shaders> shader;
        std::unordered_map<u64, Blend> blend;
        GLuint mvpVar{static_cast<GLuint>(-1)};
        glm::mat4 mvp{};

        GLuint texture{};
        GLuint ebo{};
        GLuint vbo{};
    };
}
