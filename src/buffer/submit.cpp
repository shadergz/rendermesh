#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <SDL_image.h>

#include <buffer/submit.h>
namespace rendermesh::buffer {
    void Submit::bindBuffer(const std::vector<Vertex>& triangles, const std::vector<GLuint>& indices) const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangles[0]) * triangles.size(), &triangles[0], GL_STATIC_DRAW);

        // We are using the AOS memory model
        glEnableVertexAttribArray(positionsAttr);
        glVertexAttribPointer(positionsAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

        glEnableVertexAttribArray(normalsAttr);
        glVertexAttribPointer(normalsAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

        glEnableVertexAttribArray(texturesAttr);
        glVertexAttribPointer(texturesAttr, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texture)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    void Submit::reset() {
        blend.clear();
        buffers.reset();
    }

    void Submit::accMvp(const glm::mat4& point, const bool isView) {
        if (mvpVar == -1) {
            mvpVar = shader->getUniform("mvp");
            mvp = point;
        } else if (isView) {
            mvp = point;
        } else {
            mvp *= point;
        }

        glUniformMatrix4fv(mvpVar, 1, GL_FALSE, &mvp[0][0]);
    }

    void Submit::drawBuffers(const u64 meshHash, const u32 indices) {
        // Binds the texture to our object in the fragment shader
        const auto& materials{blend[meshHash]};
        if (!blend.contains(meshHash))
            throw std::runtime_error("Blend not in use");

        materials.bind(shader, aiTextureType_DIFFUSE);
        materials.bind(shader, aiTextureType_SPECULAR);
        materials.bind(shader, aiTextureType_EMISSIVE);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void Submit::loadTexture(const u64 meshHash, const Texture& resource) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        const auto source{IMG_Load(resource.path.c_str())};
        SDL_Rect imageFrame{0, 0, source->w, source->h};
        uint32_t redMask;
        uint32_t greenMask;
        uint32_t blueMask;
        uint32_t alphaMask;
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        redMask = 0xff000000;
        greenMask = 0x00ff0000;
        blueMask = 0x0000ff00;
        alphaMask = 0x000000ff;
#else
        redMask = 0x000000ff;
        greenMask = 0x0000ff00;
        blueMask = 0x00ff0000;
        alphaMask = 0xff000000;
#endif

        const auto target{SDL_CreateRGBSurface(0, imageFrame.w, imageFrame.h, 32,
            redMask, greenMask, blueMask, alphaMask)};

        mixTexture(meshHash, resource);

        SDL_BlitSurface(source, &imageFrame, target, &imageFrame);

        if (target->pixels) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageFrame.w, imageFrame.h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, target->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        SDL_FreeSurface(target);
        SDL_FreeSurface(source);
    }

    void Submit::mixTexture(const u64 meshHash, const Texture& resource) {
        if (blend.contains(meshHash)) {
            blend[meshHash].blending(resource.type, texture);
        } else {
            blend[meshHash] = Blend(resource.type, texture);
        }
        blend[meshHash].bind(shader, resource.type);
    }

    void Submit::bindMeshModel(const u32 model) {
        texture = buffers[model].texture;
        ebo = buffers[model].ebo;
        vbo = buffers[model].vbo;
    }
}
