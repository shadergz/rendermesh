#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <SDL_image.h>

#include <meshes_buffer.h>
namespace rendermesh {
    MeshesBuffer::~MeshesBuffer() {
        glBindVertexArray(0);

        glDeleteTextures(1, &texture);
        glDeleteBuffers(2, &vbos[0]);
        glDeleteVertexArrays(1, &vao);
    }

    void MeshesBuffer::bind() const {
        glBindVertexArray(vao);
    }
    void MeshesBuffer::createVAO() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(2, &vbos[0]);
    }

    void MeshesBuffer::bindBuffer(const std::vector<Vertex>& triangles, const std::vector<GLuint>& indices) const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[PositionVBO]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangles[0]) * triangles.size(), &triangles[0], GL_STATIC_DRAW);

        // We are using the AOS memory model
        // ReSharper disable once CppRedundantCastExpression
        glVertexAttribPointer(positionsAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(positionsAttr);

        // ReSharper disable once CppRedundantCastExpression
        glVertexAttribPointer(normalsAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(normalsAttr);

        // ReSharper disable once CppRedundantCastExpression
        glVertexAttribPointer(texturesAttr, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texture)));
        glEnableVertexAttribArray(texturesAttr);

        glBindVertexArray(0);
    }

    void MeshesBuffer::loadTexture([[maybe_unused]] const std::filesystem::path& path) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        const auto source{IMG_Load(path.c_str())};
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

        const auto target{SDL_CreateRGBSurface(0,imageFrame.w, imageFrame.h, 32,
            redMask, greenMask, blueMask, alphaMask)};

        SDL_BlitSurface(source, &imageFrame, target, &imageFrame);
        SDL_FreeSurface(source);

        if (target->pixels) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageFrame.w, imageFrame.h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, target->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        SDL_FreeSurface(target);
    }

    void MeshesBuffer::draw(const GLsizei indices) const {
        // Binds the texture to our object in the fragment shader 'tex'
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, nullptr);
    }
}