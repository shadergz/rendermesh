cmake_minimum_required(VERSION 3.21)
project(RenderMesh)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE -Ofast)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)

add_subdirectory(external/tinyobjloader)
add_subdirectory(external/glm)
find_package(OpenGL REQUIRED)
include_directories(.)

add_executable(rendermesh)
target_sources(rendermesh PRIVATE
    main.cpp
    complex_model.cpp
    complex_model.h
    meshes_buffer.cpp
    meshes_buffer.h
    main_window.cpp
    main_window.h
    render.cpp
    render.h
    shaders.cpp
    shaders.h
    camera.cpp
    camera.h
    types.h
)

set(INGUI_DIR external/imgui)
add_library(imgui STATIC
    ${INGUI_DIR}/imgui.cpp
    ${INGUI_DIR}/imgui_draw.cpp
    ${INGUI_DIR}/imgui_tables.cpp
    ${INGUI_DIR}/imgui_widgets.cpp

    ${INGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${INGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${INGUI_DIR})
target_link_libraries(imgui PUBLIC GL SDL2::SDL2)

target_link_libraries(rendermesh PRIVATE SDL2_image::SDL2_image tinyobjloader glm imgui)
