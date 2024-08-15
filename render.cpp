#include <string>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <imgui.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>

#include <render.h>
namespace rendermesh {
    constexpr auto maxOfLoadableModel{1};
    Render::Render(MainWindow& main) :
        window(main) {

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        buffers.resize(maxOfLoadableModel);

        for (auto& modelBuffers : buffers) {
            glGenBuffers(1, &modelBuffers.ebo);
            glGenBuffers(1, &modelBuffers.vbo);

            glGenTextures(1, &modelBuffers.texture);
        }

        i32 h, w;
        SDL_GetWindowSize(window.main, &w, &h);

        shader = std::make_unique<Shaders>();
        mvp = shader->getMvpVar();
        buffer = std::make_shared<MeshesBuffer>(buffers);
        camera = Camera(static_cast<f32>(h), static_cast<f32>(w));
    }

    void Render::render() {
        bool quit{};
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);

        window.walk = [&](const SDL_Keysym& kb) {
            camera.walkAround(kb);
        };
        window.look = [&](const f32 x, const f32 y) {
            camera.lookAround(x, y);
        };
        window.enbVerticesView = [](const u32 state) {
            static bool isEnb{};
            if (state == SDL_KEYDOWN)
                return;

            if (isEnb) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                isEnb = {};
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                isEnb = true;
            }
        };
        using FasterClock = std::chrono::time_point<std::chrono::high_resolution_clock>;
        using DiffTicks = std::chrono::duration<f32>;
        using GetTicks = std::chrono::high_resolution_clock;

        FasterClock beginTime{};
        FasterClock endTime{};
        f32 deltaTime{};

        std::array<f32, 200> framesSamples{};
        u32 sample{};
        std::array bgColor{0.f, 0.f, 0.125f, 1.f};
        f32 cs{0.05f};

        while (!quit) {
            beginTime = GetTicks::now();
            camera.setCameraSpeed(cs);
            window.receiveEvents(quit);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            bool isActivate{};
            ImGui::Begin("Scene", &isActivate, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            const auto fps{deltaTime * 1000.f};
            if (sample > sizeof(framesSamples) / sizeof(f32))
                sample = 0;

            ImGui::ColorEdit4("Background", &bgColor[0]);
            ImGui::SliderFloat("Camera Speed", &cs, 0.1f, 1.f);

            framesSamples[sample++] = fps;
            ImGui::PlotLines("Framerate", &framesSamples[0], framesSamples.size());

            ImGui::Text("FPS: %f", fps);
            ImGui::End();

            const auto view{camera.getViewMatrix()};
            draw(view);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glFlush();
            endTime = GetTicks::now();
            SDL_GL_SwapWindow(window.main);

            deltaTime = DiffTicks(endTime - beginTime).count();
        }
    }

    void Render::initialize(const std::vector<char*>& args) {
        std::string objectPath{};
        for (const auto argument : args) {
            if (std::string(".obj") == std::filesystem::path(argument).extension())
                objectPath = argument;
        }
        if (objectPath.empty())
            std::terminate();
        const std::filesystem::path meshFile(objectPath);
        if (!exists(meshFile)) {

        }
        mesh = std::make_unique<ComplexModel>(buffer, meshFile);
        mesh->populateBuffers();
    }
    void Render::draw(const glm::mat4& view) const {
        // All the drawing work is done here
        glBindVertexArray(vao);
        glUniformMatrix4fv(mvp, 1, GL_FALSE, &view[0][0]);

        shader->useShaders();
        if (mesh)
            mesh->draw();

        glBindVertexArray(0);
    }

    glm::mat4 Render::transformMesh() {
        constexpr glm::mat4 identity{1.0f};
        constexpr glm::vec3 position{0.0f, 0.0f, 0.0f};
        constexpr glm::vec3 rotationAxis{0.0f, 1.0f, 0.0f};
        constexpr glm::vec3 scale{1.0f, 1.0f, 1.0f};
        constexpr float rotationDegrees{45.0f};

        return translate(identity, position) *
            rotate(identity, glm::radians(rotationDegrees), rotationAxis) * glm::scale(identity, scale);
    }
}
