#pragma once
#include <SDL_keyboard.h>
#include <glm/glm.hpp>

#include <types.h>
namespace rendermesh::view {
    class Camera {
    public:
        Camera() = default;
        Camera(f32 width, f32 height);

        glm::mat4 getViewMatrix();
        void walkAround(const SDL_Keysym& board);
        void lookAround(f32 x, f32 y);

        void setCameraSpeed(f32 value);
    private:
        glm::vec3 cameraPos{0.0f, 0.0f,  3.0f};
        glm::vec3 cameraFront{0.0f, 0.0f, -1.0f};
        glm::vec3 cameraUp{0.0f, 1.0f, 0.0f};

        glm::mat4 projection{};
        glm::mat4 view{};

        glm::vec2 oldMouse{};
        f32 yaw{}, pitch{};

        f32 cameraSpeed{};
    };
}
