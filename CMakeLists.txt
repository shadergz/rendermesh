cmake_minimum_required(VERSION 3.21)
project(RenderMesh)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE -Ofast)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)

add_subdirectory(external/tinyobjloader)
add_subdirectory(external/glm)
add_subdirectory(external/ImGuiFileDialog)
find_package(OpenGL REQUIRED)

add_executable(rendermesh)
add_subdirectory(src)

set(IMGUI_DIR external/imgui)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_link_libraries(imgui PUBLIC GL SDL2::SDL2)

target_link_libraries(ImGuiFileDialog PUBLIC imgui)
target_link_libraries(rendermesh PRIVATE SDL2_image::SDL2_image tinyobjloader glm ImGuiFileDialog)
