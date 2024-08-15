#include <stdexcept>
#include <string>

#include <main_window.h>
#include <SDL_image.h>
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
        IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
        main = SDL_CreateWindow("Mesh Loader", 0, 0, width, height, windowFlags);
        if (!main)
            throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
        context = SDL_GL_CreateContext(main);
    }

    u32 MainWindow::receiveEvents(bool& quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                event.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN)
                if (event.key.keysym.sym == SDL_SCANCODE_ESCAPE)
                    quit = true;

            if (event.key.keysym.sym == SDLK_e)
                enbVerticesView(event.type);

            if (SDL_MOUSEMOTION == event.type && SDL_BUTTON_LEFT == event.button.button) {
                i32 x, y;
                SDL_GetMouseState(&x, &y);
                look(static_cast<f32>(x), static_cast<f32>(y));
            }
            walk(event.key.keysym);
        }
        if (quit)
            running = {};
        return {};
    }
}
