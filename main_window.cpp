#include <stdexcept>
#include <string>

#include <main_window.h>
namespace rendermesh {
    MainWindow::~MainWindow() {
        // Frees memory
        SDL_DestroyWindow(main);
        SDL_Quit();
    }

    void MainWindow::create() {
        constexpr u32 windowFlags{SDL_WINDOW_OPENGL};
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error("SDL Failed: " + std::string(SDL_GetError()));
        }
        main = SDL_CreateWindow("Mesh GL", 0, 0, width, height, windowFlags);
        if (!main)
            throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
        context = SDL_GL_CreateContext(main);
    }

    u32 MainWindow::receiveEvents(bool& quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN)
                if (event.key.keysym.sym == SDL_SCANCODE_ESCAPE)
                    quit = true;
        }
        if (quit)
            running = {};
        return {};
    }
}
