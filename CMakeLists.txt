cmake_minimum_required(VERSION 3.21)
project(RenderMesh)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)

add_subdirectory(external/tinyobjloader)
add_subdirectory(external/glm)
find_package(OpenGL REQUIRED)
include_directories(.)

add_executable(rendermesh)
target_sources(rendermesh PRIVATE
    main.cpp
    complex_mesh.cpp
    complex_mesh.h
    mesh_buffer.cpp
    mesh_buffer.h
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

if(TARGET SDL2::SDL2main)
    target_link_libraries(rendermesh PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(rendermesh PRIVATE SDL2::SDL2 SDL2_image::SDL2_image tinyobjloader glm GL)
