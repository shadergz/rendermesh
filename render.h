#pragma once
#include <vector>

#include <main_window.h>
#include <complex_mesh.h>

namespace rendermesh {
    class Render {
    public:
        explicit Render(MainWindow& main) : window(main) {}
        void render() const;
        void initialize(const std::vector<char*>& args);

        void draw() const;

        MainWindow& window;
        std::unique_ptr<ComplexMesh> mesh;
        Shaders shader;
    };
}
