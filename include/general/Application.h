#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

// Forward declarations
class Camera;
class Renderer;
class Chunk;
class Waterfall;
class Rain;
class VolumetricFog;
class VertexArrayWrapper;
class Shader;

/**
 * @brief Main application class - eliminates all global variables
 *
 * Encapsulates entire application state using composition.
 * Follows Single Responsibility Principle - manages application lifecycle.
 */
class Application {
private:
    // Window
    GLFWwindow* window;

    // Rendering
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<VertexArrayWrapper> worldVAO;
    std::unique_ptr<VertexArrayWrapper> lightVAO;
    std::unique_ptr<Shader> worldShader;
    std::unique_ptr<Shader> lightShader;

    // Camera
    std::unique_ptr<Camera> camera;

    // Input state
    bool firstMouse;
    float lastX, lastY;

    // Timing
    float deltaTime;
    float lastFrame;

    // World
    std::vector<std::vector<std::unique_ptr<Chunk>>> chunks;
    std::unique_ptr<Waterfall> waterfall;

    // Effects
    std::unique_ptr<Rain> rain;
    std::unique_ptr<VolumetricFog> fog;

    // Private methods
    void initWindow();
    void initShaders();
    void initWorld();
    void initEffects();
    void updateEffects();
    void renderWorld();
    void renderEffects();
    void renderLights();

public:
    Application();
    ~Application();

    /**
     * @brief Initialize application
     */
    void init();

    /**
     * @brief Run main loop
     */
    void run();

    /**
     * @brief Clean up resources
     */
    void shutdown();

    // Input callbacks (need access to camera)
    void processKeyboard(float dt);
    void processMouse(double xpos, double ypos);
    void processScroll(double yoffset);

    // Prevent copying
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};
