#define GL_GLEXT_PROTOTYPES
#include <exception>
#include <iostream>
#include <SDL2/SDL_opengl.h>
#include <shaders.h>
#include <stdexcept>

#include <types.h>
#include <vector>

namespace rendermesh {

    static std::string vertex{
        "#version 440\n"
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        "layout (location = 2) in vec2 texture;\n"
        "\n"
        "\n"
        "uniform mat4 mvp;\n"
        "out vec3 colors;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = mvp * vec4(position, 1.f);\n"
        "   colors = normal;\n"
        "   TexCoord = texture;\n"
        "}\n"
    };
    static std::string fragment {
        "#version 440\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "in vec3 colors;\n"
        "\n"
        "uniform sampler2D tex;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(tex, TexCoord);\n"
        "}\n"
    };

    Shaders::Shaders() {
        vertexShader = {glCreateShader(GL_VERTEX_SHADER)};
        fragmentShader = {glCreateShader(GL_FRAGMENT_SHADER)};

        compileShader(vertexShader, vertex);
        compileShader(fragmentShader, fragment);

        program = glCreateProgram();

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        if (glGetError())
            std::terminate();
    }

    Shaders::~Shaders() {
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        if (glIsProgram(program))
            glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Shaders::useShaders() const {
        if (!glIsProgram(program)) {
            throw std::runtime_error("");
        }
        glUseProgram(program);
    }

    GLuint Shaders::getMvpVar() const {
        return glGetUniformLocation(program, "mvp");
    }

    void Shaders::compileShader(const u32& shader, const std::string& source) {
        const auto src{source.c_str()};

        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status)
            return;

        std::vector<GLchar> errors(100);
        const GLsizei esz{static_cast<GLsizei>(errors.size())};
        glGetShaderInfoLog(shader, esz, nullptr, &errors[0]);
        std::cout << std::string(&errors[0]) << '\n';
        std::terminate();
    }
}