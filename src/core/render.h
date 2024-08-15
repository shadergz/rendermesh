#pragma once
#include <array>

#include <window/main_window.h>
#include <mesh/complex.h>
#include <view/camera.h>

#include <buffer/pipeline.h>
namespace rendermesh::core {
    class Render {
    public:
        explicit Render(window::MainWindow& main);
        void render();
        void initialize(const std::vector<char*>& args);
        void open(const std::filesystem::path& path);

        void drawUi();
        void draw(const glm::mat4& view);

        static glm::mat4 transformMesh();
        window::MainWindow& window;
        std::unique_ptr<mesh::Complex> mesh;

        std::shared_ptr<buffer::Submit> submitter;
        view::Camera camera;

        std::shared_ptr<raster::Shaders> shader;

        buffer::Pipeline pipeline;
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
        std::array<float, 4> bgColor{.06f, .90f, .98f, 1.f};
        f32 cs{0.05f};

        std::filesystem::path selected;
    };
}
