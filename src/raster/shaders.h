#pragma once
#include <string>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <types.h>
namespace rendermesh::raster {
    class Shaders {
    public:
        Shaders();
        ~Shaders();
        void activate() const;
        void drop() const;

        GLuint getUniform(std::string_view property) const;
        bool isCurrent() const;

    private:
        static void compileShader(const u32& shader, const std::string& source);

        u32 program{};
        u32 vertexShader{};
        u32 fragmentShader{};
    };
}
