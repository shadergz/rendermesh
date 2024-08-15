#pragma once

#include <string>
#include <types.h>
namespace rendermesh {
    class Shaders {
    public:
        Shaders();
        ~Shaders();
        void useShaders() const;

        GLuint getMvpVar() const;

        u32 program{};
    private:
        static void compileShader(const u32& shader, const std::string& source);
        u32 vertexShader{};
        u32 fragmentShader{};
    };
}
