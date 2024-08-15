#include <stdexcept>
#include <utility>

#include <buffer/pipeline.h>
namespace rendermesh::buffer {
    MeshPipeline::~MeshPipeline() {
        if (vbo)
            glDeleteBuffers(1, &vbo);
        if (ebo)
            glDeleteBuffers(1, &ebo);

        if (texture)
            glDeleteTextures(1, &texture);
    }

    MeshPipeline::MeshPipeline(MeshPipeline&& last) noexcept {
        vbo = last.vbo;
        ebo = last.ebo;
        texture = last.texture;

        last.vbo = last.ebo = last.texture = {};
    }

    u32 Pipeline::generate() {
        MeshPipeline buffer{};

        glGenBuffers(1, &buffer.ebo);
        glGenBuffers(1, &buffer.vbo);

        glGenTextures(1, &buffer.texture);
        const auto pipe{pipes.size()};

        if (enbFiltering) {
            glBindTexture(GL_TEXTURE_2D, buffer.texture);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        pipes.emplace_back(std::move(buffer));
        return pipe;
    }

    void Pipeline::reuse(const u32 dest, const GLuint src) {
        auto& target{operator[](dest).texture};
        const auto& source{operator[](src).texture};

        if (target != source)
            glDeleteTextures(1, &target);

        target = source;
    }

    void Pipeline::reset() {
        pipes.clear();
    }

    MeshPipeline& Pipeline::operator[](const u32 index) {
        auto position{std::begin(pipes)};

        std::advance(position, index);
        if (position == std::end(pipes)) {
            throw std::runtime_error("Out of bounds access");
        }
        return std::forward<MeshPipeline&>(*position);
    }

    void Pipeline::bind() const {
        glBindVertexArray(vao);
    }
    void Pipeline::flush() const {
        if (vao)
            glFlush();
        glBindVertexArray(0);
    }
}
