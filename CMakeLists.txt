cmake_minimum_required(VERSION 3.27)
project(Starter CXX)

build_application(

    SOURCES
    ./applications/learnopengl_camera_demo.cpp
    ./applications/Block.cpp
    ./applications/CameraWrapper.cpp
    ./applications/Chunk.cpp
    ./applications/noiseutils.cpp
    ./applications/Renderer.cpp
    ./applications/VertexArrayWrapper.cpp

    PACKAGES
    glfw3
    fmt
    glad
    libnoise

    LINK_PACKAGES
    glfw
    fmt::fmt
    glad::glad
    libnoise::libnoise
)

target_include_directories(${PROJECT_NAME} PUBLIC "include")

generate_compile_commands()