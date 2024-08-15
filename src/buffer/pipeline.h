#pragma once
#include <list>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <types.h>
namespace rendermesh::buffer {

    struct MeshPipeline {
        MeshPipeline() = default;
        ~MeshPipeline();

        explicit MeshPipeline(MeshPipeline&& last) noexcept;

        GLuint vbo{};
        GLuint ebo{};
        GLuint texture{};
    };

    class Pipeline {
    public:
        Pipeline() = default;
        explicit Pipeline(const bool anisotropic, const GLfloat value) :
            enbFiltering(anisotropic), amount(value) {}

        u32 generate();
        void reuse(u32 dest, u32 src);
        void reset();

        MeshPipeline& operator[](u32 index);

        void bind() const;
        void flush() const;

    private:
        bool enbFiltering{false};

        GLfloat amount{};
        GLuint vao{};
        std::list<MeshPipeline> pipes;
    };
}
