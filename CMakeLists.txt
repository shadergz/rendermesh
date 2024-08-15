cmake_minimum_required(VERSION 3.21)
project(RenderMesh)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/tinyobjloader)
add_subdirectory(external/glm)
find_package(OpenGL REQUIRED)
include_directories(.)

add_executable(rendermesh)
target_sources(rendermesh PRIVATE
    main.cpp
    complex_mesh.cpp
    complex_mesh.h
    main_window.cpp
    main_window.h
    render.cpp
    render.h
    shaders.cpp
    shaders.h
    types.h
)

if(TARGET SDL2::SDL2main)
    target_link_libraries(rendermesh PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(rendermesh PRIVATE SDL2-static tinyobjloader glm GL)
