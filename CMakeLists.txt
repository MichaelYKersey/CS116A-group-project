cmake_minimum_required(VERSION 3.27)
project(Starter CXX)



build_application(

    SOURCES
    ./applications/learnopengl_camera_demo.cpp

    PACKAGES
    glfw3
    fmt
    glad

    LINK_PACKAGES
    glfw
    fmt::fmt
    glad::glad
)

target_include_directories(${PROJECT_NAME} PUBLIC "include")

generate_compile_commands()