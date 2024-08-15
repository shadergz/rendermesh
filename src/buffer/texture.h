#pragma once

#include <filesystem>
#include <assimp/material.h>

namespace rendermesh::buffer {
    struct Texture {
        std::filesystem::path path;
        aiTextureType type;
    };
}