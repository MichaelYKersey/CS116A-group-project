cmake_minimum_required(VERSION 3.27)
project(Starter CXX)

build_application(

    SOURCES
    ./src/general/CameraWrapper.cpp
    ./src/general/Renderer.cpp
    ./src/general/VertexArrayWrapper.cpp
    ./src/general/app_util.cpp
    ./src/terrain/Block.cpp
    ./src/terrain/Chunk.cpp
    ./src/noise/noiseutils.cpp
    
    # comment out all applications besides the one you want to run
    ./applications/main.cpp
    #./applications/learnopengl_camera_demo.cpp

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