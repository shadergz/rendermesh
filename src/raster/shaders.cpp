#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <types.h>
#include <raster/shaders.h>
namespace rendermesh::raster {

    static std::string vertex{
        "#version 440\n"
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        "layout (location = 2) in vec2 texture;\n"
        "\n"
        "\n"
        "uniform mat4 mvp;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = mvp * vec4(position, 1.f);\n"
        "   TexCoord = texture;\n"
        "}\n"
    };
    static std::string fragment {
        "#version 440\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "\n"
        "uniform sampler2D diffuse;\n"
        "uniform sampler2D specular;\n"
        "uniform sampler2D emissive;\n"
        "void main()\n"
        "{\n"
        "   vec3 diff = vec3(texture(diffuse, TexCoord));\n"
        "   vec3 spec = vec3(texture(specular, TexCoord));\n"
        "   vec3 emi = vec3(texture(emissive, TexCoord));\n"
        "   FragColor = vec4(diff + spec + emi, 1.0);\n"
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

        program = vertexShader = fragmentShader = {};
    }

    void Shaders::activate() const {
        if (!glIsProgram(program)) {
            throw std::runtime_error("");
        }
        glUseProgram(program);
    }

    void Shaders::drop() const {
        if (isCurrent())
            glUseProgram(0);
    }

    GLuint Shaders::getUniform(const std::string_view property) const {
        if (!isCurrent())
            throw std::runtime_error("No shaders have been loaded yet");

        return glGetUniformLocation(program, property.data());
    }

    bool Shaders::isCurrent() const {
        GLint running;
        glGetIntegerv(GL_CURRENT_PROGRAM, &running);
        return program == running;
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
