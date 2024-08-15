
#include <core/render.h>
int main(const int argc, char** argv) {
    namespace rm = rendermesh;
    rm::window::MainWindow window;
    window.create();

    rm::core::Render graphics(window);
    std::vector<char*> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    graphics.initialize(args);
    graphics.render();

    return {};
}
