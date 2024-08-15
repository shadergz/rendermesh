#include <camera.h>

#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace rendermesh {
    Camera::Camera(const f32 width, const f32 height) {
        static constexpr auto fieldOfView{60.f};
        projection = glm::perspective(
            glm::radians(fieldOfView), width / height, 0.01f, 100.0f);
    }

    glm::mat4 Camera::getViewMatrix() {
        view = lookAt(cameraPos,
            cameraPos + cameraFront, cameraUp);
        return projection * view;
    }

    void Camera::walkAround(const SDL_Keysym& board) {
        if (board.sym == SDLK_w)
            cameraPos += cameraSpeed * cameraFront;
        if (board.sym == SDLK_s)
            cameraPos -= cameraSpeed * cameraFront;
        if (board.sym == SDLK_a)
            cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
        if (board.sym == SDLK_d)
            cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    void Camera::lookAround(const f32 x, const f32 y) {
        static auto initMouse{true};
        const glm::vec2 mouse{x, y};
        if (initMouse) {
            oldMouse = mouse;
            initMouse = {};
        }

        glm::vec2 mouseDelta{oldMouse - mouse};
        mouseDelta.y = oldMouse.y - mouse.y;
        oldMouse = mouse;

        constexpr auto sensitivity{0.305f};
        mouseDelta *= sensitivity;

        yaw += mouseDelta.x;
        pitch += mouseDelta.y;

        if (pitch > 90.0f)
            pitch = 90.0f;
        if (pitch < -90.0f)
            pitch = -90.0f;

        const glm::vec3 direction{
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch)),
        };
        cameraFront = normalize(direction);
    }

    void Camera::setCameraSpeed(const f32 value) {
        cameraSpeed = value;
    }
}
