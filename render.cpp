#include <string>
#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#include <render.h>

namespace rendermesh {
    void Render::render() const {
        bool quit{};
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_MULTISAMPLE);
        glViewport(0, 0, width, height);

        while (!quit) {
            glClearColor(0.f, 0.f, 0.f, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            window.receiveEvents(quit);
            draw();
            SDL_GL_SwapWindow(window.main);
            glFlush();
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
        mesh = std::make_unique<ComplexMesh>(shader, meshFile);
        mesh->populateBuffers();
    }
    void Render::draw() const {
        // All the drawing work is done here
        mesh->draw();
        glBindVertexArray(0);
    }
}
