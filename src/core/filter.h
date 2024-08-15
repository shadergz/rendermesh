#pragma once

#include <utility>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

namespace rendermesh::core {
    std::pair<bool, GLfloat> getAnisotropicFilter();
}