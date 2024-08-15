#pragma once
#include <functional>
#include <SDL2/SDL.h>

#include <types.h>
namespace rendermesh::window {
    constexpr auto width{700};
    constexpr auto height{700};

    class MainWindow {
    public:
        MainWindow() = default;
        ~MainWindow();
        void create();

        u32 receiveEvents(bool& quit);
        SDL_Window* main{};

        std::function<void(u32)> enbVerticesView;
        std::function<void(const SDL_Keysym&)> walk;
        std::function<void(f32 x, f32 y)> look;

    private:
        SDL_GLContext context{};
        bool running{true};
    };
}
