cmake_minimum_required(VERSION 3.21)

target_include_directories(rendermesh PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

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