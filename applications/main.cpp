#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>

#include <iostream>

#include <noise/noise.h>
#include "noiseutils.h"

#include <VertexArrayWrapper.h>
#include <Renderer.h>
#include <CameraWrapper.h>
#include <Chunk.h>
#include <app_util.hpp>

using namespace noise;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void renderWorld(VertexArrayWrapper &worldVAO, Shader worldShader, Renderer renderer, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection, glm::vec4 plane);

// settings
const int WORLD_SIZE          = 16;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// Create Camera
Camera camera(cameraPos, cameraUp, -90.0f, 0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Light
glm::vec3 lightPos(10.0f, 10.0f, -10.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

int main()
{
    GLFWwindow* window = create_window();
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    Shader worldShader("./shaders/WorldShader.GLSL");
    Shader lightShader("./shaders/LightsShader.GLSL");
    lightShader.use();

    // Init Renderer
    Renderer renderer;

    // Create World Vertex Array
    VertexArrayWrapper worldVAO(Vertex_Normal_RGB_Optimized);

    // Set up world
    std::vector<std::vector<std::unique_ptr<Chunk>>> chunks(WORLD_SIZE);
    
    // Iterate through chunk grid
    for (int i = 0; i < WORLD_SIZE; i++) {
        for (int j = 0; j < WORLD_SIZE; j++) {
            std::string key = "Chunk" + std::to_string(i) + std::to_string(j);
            std::unique_ptr<Chunk> chunkPtr = std::make_unique<Chunk>();
            chunks[i].push_back(std::move(chunkPtr));
            chunks[i][j]->createLandscape(chunks[i][j]->CHUNK_SIZE * (i + 2), chunks[i][j]->CHUNK_SIZE * (j + 2));
            worldVAO.createVBO(key, chunks[i][j]->render());
        }
    }

    std::cout << "iterated through chunks" << std::endl;
    
    // Setup lights
    std::vector<float> lights = {
        // positions          
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f, 0.5f,
         0.5f, -0.5f, 0.5f,
         0.5f,  0.5f, 0.5f, 
         0.5f,  0.5f, 0.5f,
        -0.5f,  0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, 

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f, 

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f, 
    };
    VertexArrayWrapper lightVAO(Vertex_Default);
    lightVAO.createVBO("Light", lights);
    lightVAO.bindVBO("Light");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Enable Clipping Plane
        glEnable(GL_CLIP_DISTANCE0);

        // Initialize Transformation Matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 200.0f);
        
        // Render Lights
        lightShader.use();
        lightVAO.bind();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightVAO.bindVBO("Light");
        renderer.draw(lightVAO, lightShader);

        // Generate World
        glDisable(GL_CLIP_DISTANCE0);
        renderWorld(worldVAO, worldShader, renderer, model, view, projection, glm::vec4(0, 0, 0, 0));

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // // optional: de-allocate all resources once they've outlived their purpose:
    // // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void renderWorld(VertexArrayWrapper &worldVAO, Shader worldShader, Renderer renderer, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection, glm::vec4 plane) {
    worldVAO.bind();
    worldShader.use();
    worldShader.setVec4("plane", plane);
    worldShader.setMat4("view", view);
    worldShader.setMat4("projection", projection);
    worldShader.setVec3("lightPos", lightPos);
    worldShader.setVec3("lightColor", lightColor);
    worldShader.setVec3("viewPos", camera.position);

    for (int i = 0; i < WORLD_SIZE; ++i) {
        for (int j = 0; j < WORLD_SIZE; ++j) {
            std::string key = "Chunk" + std::to_string(i) + std::to_string(j);
            std::unique_ptr<Chunk> chunkPtr = std::make_unique<Chunk>();
            worldVAO.bindVBO(key);

            // Draw
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(20, 20, 20));
            model = glm::translate(model, glm::vec3(i, 0.0f, -j));
            worldShader.setMat4("model", model);

            renderer.draw(worldVAO, worldShader);
        }
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CamMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CamMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CamMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CamMovement::RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (firstMouse)
    {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = false;
    }

    float xoffset = xposIn - lastX;
    float yoffset = lastY - yposIn; // reversed since y-coordinates go from bottom to top
    lastX = xposIn;
    lastY = yposIn;

    yaw += xoffset;
    pitch += yoffset;

    camera.processMouseMovement(xoffset, yoffset, false);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}