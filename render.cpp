#include <string>
#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#include <render.h>

namespace rendermesh {
    Render::Render(MainWindow& main) :
        window(main) {

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(2, &vbos[0]);

        i32 h, w;
        SDL_GetWindowSize(window.main, &w, &h);

        shader = std::make_unique<Shaders>();
        mvp = shader->getMvpVar();
        buffer = std::make_shared<MeshBuffer>(vbos);
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

        while (!quit) {
            window.receiveEvents(quit);

            glClearColor(0.f, 0.f, 0.f, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const auto view{camera.getViewMatrix() * transformMesh()};
            draw(view);

            glFlush();
            SDL_GL_SwapWindow(window.main);
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
        mesh = std::make_unique<ComplexMesh>(buffer, meshFile);
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
