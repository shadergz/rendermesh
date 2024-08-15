#pragma once
#include <list>
#include <array>

#include <main_window.h>
#include <complex_model.h>
#include <camera.h>

namespace rendermesh {
    class Render {
    public:
        explicit Render(MainWindow& main);
        void render();
        void initialize(const std::vector<char*>& args);
        void open(const std::filesystem::path& path);
        void drawUi();

        void draw(const glm::mat4& view) const;

        static glm::mat4 transformMesh();
        MainWindow& window;
        std::unique_ptr<ComplexModel> mesh;

        std::shared_ptr<MeshesBuffer> buffer;
        Camera camera;

        std::unique_ptr<Shaders> shader;

        GLuint mvp{};
        GLuint vao{};
        std::list<MeshPipelineBuffers> buffers;
        std::vector<std::filesystem::path> files;

        using FasterClock = std::chrono::time_point<std::chrono::high_resolution_clock>;
        using GetTicks = std::chrono::high_resolution_clock;

        FasterClock beginTime{};
        FasterClock endTime{};
        f32 deltaTime{};
        f32 acc{};
        f32 fps{};
        f32 frames{};

        std::array<f32, 200> framesSamples{};
        u32 sample{};
        std::array<float, 4> bgColor{0.f, 0.f, 0.125f, 1.f};
        f32 cs{0.05f};

        std::filesystem::path selected;
    };
}
