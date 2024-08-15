#pragma once
#include <vector>

#include <main_window.h>
#include <complex_mesh.h>
#include <camera.h>

namespace rendermesh {
    class Render {
    public:
        explicit Render(MainWindow& main);
        void render();
        void initialize(const std::vector<char*>& args);

        void draw(const glm::mat4& view) const;

        static glm::mat4 transformMesh();
        MainWindow& window;
        std::unique_ptr<ComplexMesh> mesh;

        std::shared_ptr<MeshBuffer> buffer;
        Camera camera;

        std::unique_ptr<Shaders> shader;

        GLuint mvp{};
        GLuint vao{};
        std::array<GLuint, EBO + 1> vbos{};
    };
}
