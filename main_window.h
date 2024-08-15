#pragma once
#include <SDL2/SDL.h>

#include <types.h>
namespace rendermesh {
    constexpr auto width{600};
    constexpr auto height{600};

    class MainWindow {
    public:
        MainWindow() = default;
        ~MainWindow();
        void create();

        u32 receiveEvents(bool& quit);
        SDL_Window* main{};
    private:
        SDL_GLContext context{};
        bool running{true};
    };
}
