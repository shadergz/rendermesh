#include <string>
#include <chrono>
#include <print>

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <ImGuiFileDialog.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>

#include <render.h>
namespace rendermesh {
    constexpr auto maxOfLoadableModel{12};
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

        SDL_ShowWindow(window.main);

        while (!quit) {
            beginTime = GetTicks::now();
            camera.setCameraSpeed(cs);
            window.receiveEvents(quit);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            drawUi();

            const auto view{camera.getViewMatrix()};
            draw(view);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glFlush();
            endTime = GetTicks::now();
            SDL_GL_SwapWindow(window.main);

            frames++;

            deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
        }
    }

    void Render::initialize(const std::vector<char*>& args) {
        for (const auto argument : args) {
            for (const auto desired : {".obj", ".gltf", ".glb"}) {
                if (std::string_view(desired) == std::filesystem::path(argument).extension()) {
                    const auto file{std::filesystem::canonical(argument)};
                    if (std::ranges::find(files, file) != files.end())
                        throw std::runtime_error("Already exists");
                    files.emplace_back(file);
                }
            }
        }
        for (const auto& file : files) {
            if (!exists(file))
                throw std::runtime_error("File does not exist");
        }

        if (files.size())
            open(files.back());
    }

    void Render::open(const std::filesystem::path& path) {
        if (mesh)
            mesh.reset();
        mesh = std::make_unique<ComplexModel>(buffer, path);
        mesh->populateBuffers();

        selected = path;
    }

    void Render::drawUi() {
        bool isActivate{};
        ImGui::Begin("Scene", &isActivate, ImGuiWindowFlags_MenuBar);
        const auto fileDialog{ImGuiFileDialog::Instance()};
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    const IGFD::FileDialogConfig config{.path = "."};
                    fileDialog->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj,.gltf,.glb", config);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        if (fileDialog->Display("ChooseFileDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                const std::filesystem::path path{fileDialog->GetFilePathName()};
                const auto contains{std::ranges::find(files, path)};
                if (contains == files.end())
                    files.emplace_back(path);
            }

            fileDialog->Close();
        }

        acc += deltaTime;
        if (acc > 1000.f) {
            fps = frames;
            frames = acc = {};
        }
        if (sample > sizeof(framesSamples) / sizeof(f32))
            sample = 0;

        ImGui::ColorEdit4("Background", &bgColor[0]);
        ImGui::SliderFloat("Camera Speed", &cs, 0.1f, 1.f);

        framesSamples[sample++] = fps;
        ImGui::PlotLines("Framerate", &framesSamples[0], framesSamples.size());

        ImGui::Text("FPS: %f", fps);

        ImGui::BeginTable("Recent Objects", 1);
        ImGui::TableSetupColumn("Recent Objects");

        ImGui::TableHeadersRow();
        for (u32 index{}; index < files.size(); index++) {
            ImGui::Text(files[index].c_str());

            if (files[index] == selected)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0 + index, ImGui::GetColorU32(ImVec4(0.7f, 0.3f, 0.3f, 0.65f)));

            if (ImGui::IsItemClicked()) {
                if (selected != files[index]) {
                    std::print("Selected: {}\n", files[index].string());
                    open(files[index]);
                }
            }
            ImGui::TableNextColumn();
        }

        ImGui::EndTable();

        ImGui::End();
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
