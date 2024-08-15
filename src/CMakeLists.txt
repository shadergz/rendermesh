cmake_minimum_required(VERSION 3.21)

target_include_directories(rendermesh PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(rendermesh PRIVATE
    main.cpp
    mesh/complex.cpp
    mesh/complex.h
    mesh/model.h
    buffer/submit.cpp
    buffer/submit.h
    buffer/blend.cpp
    buffer/blend.h
    buffer/pipeline.cpp
    buffer/pipeline.h
    buffer/texture.h
    buffer/vertex.h
    window/main_window.cpp
    window/main_window.h
    core/render.cpp
    core/render.h
    raster/shaders.cpp
    raster/shaders.h
    view/camera.cpp
    view/camera.h
    types.h
)