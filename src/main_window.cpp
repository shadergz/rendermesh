#include <string>
#include <print>

#include <SDL_image.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>

#include <main_window.h>
namespace rendermesh {
    constexpr auto leftButton{SDL_BUTTON_LEFT};
    MainWindow::~MainWindow() {
        // Frees memory
        SDL_DestroyWindow(main);
        SDL_Quit();

        ImGui_ImplSDL2_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();

        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(main);
        SDL_Quit();
    }

    void MainWindow::create() {
        constexpr u32 windowFlags{SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN};
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
        SDL_GL_MakeCurrent(main, context);
        // Enable vsync
        SDL_GL_SetSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        const ImGuiIO& io{ImGui::GetIO()}; (void)io;

        ImGui_ImplSDL2_InitForOpenGL(main, context);
        ImGui_ImplOpenGL3_Init();
    }

    u32 MainWindow::receiveEvents(bool& quit) {
        SDL_Event event;
        i32 mouseX, mouseY;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
                quit = true;

            const auto io{ImGui::GetIO()};
            if (event.type == SDL_MOUSEMOTION &&
                event.button.button == leftButton &&
                !io.WantCaptureMouse) {

                SDL_GetMouseState(&mouseX, &mouseY);
                look(static_cast<f32>(mouseX), static_cast<f32>(mouseY));
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = true;
                if (event.key.keysym.sym == SDLK_e)
                    enbVerticesView(event.type);
            }
            if (event.type == SDL_KEYDOWN)
                walk(event.key.keysym);
        }
        if (quit)
            running = {};
        return {};
    }
}
