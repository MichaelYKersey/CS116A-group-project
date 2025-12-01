#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>

#include <iostream>

#include <noise/noise.h>
#include <noise/noiseutils.h>

#include <general/VertexArrayWrapper.h>
#include <general/Renderer.h>
#include <general/CameraWrapper.h>
#include <general/VolumetricFog.h>
#include <general/Rain.h>
#include <general/Config.h>
#include <terrain/Chunk.h>
#include <terrain/Waterfall.h>
#include <general/app_util.hpp>
#include <general/water_plane.hpp>

using namespace noise;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);void renderWorld(VertexArrayWrapper &worldVAO, Shader& worldShader, Renderer& renderer, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection, glm::vec4& plane);

// Use Config for all settings
const int WORLD_SIZE = Config::World::SIZE;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = Config::Camera::UP_VECTOR;

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  Config::Rendering::WINDOW_WIDTH / 2.0;
float lastY =  Config::Rendering::WINDOW_HEIGHT / 2.0;
float fov   =  Config::Rendering::FOV;

// Create Camera from Config
Camera camera(Config::Camera::START_POSITION, cameraUp, Config::Camera::START_YAW, Config::Camera::START_PITCH);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Light from Config
glm::vec3 lightPos = Config::Lighting::SUN_POSITION;
glm::vec3 lightColor = Config::Lighting::SUN_COLOR;

int main()
{
    GLFWwindow* window = create_window();
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glm::mat4 view = glm::mat4(1);
    glm::mat4 projection = glm::mat4(1);
    
    
    Shader worldShader(Config::Shaders::WORLD_SHADER);
    
    // Init Renderer
    Renderer renderer;
    
    // Create World Vertex Array
    VertexArrayWrapper worldVAO(Vertex_Normal_RGB_Optimized);
    
    // Create Volumetric Fog system
    VolumetricFog fog;

    // Create Rain system
    Rain rain(WORLD_SIZE);

    // Set up world
    std::vector<std::vector<std::unique_ptr<Chunk>>> chunks(WORLD_SIZE);
    std::unique_ptr<Waterfall> waterfall;  // Waterfall at chunk (7,7)

    // Iterate through chunk grid
    for (int i = 0; i < WORLD_SIZE; i++) {
        for (int j = 0; j < WORLD_SIZE; j++) {
            std::string key = "Chunk" + std::to_string(i) +","+ std::to_string(j);
            
            // Create waterfall at configured position
            if (i == Config::World::WATERFALL_CHUNK_X && j == Config::World::WATERFALL_CHUNK_Z) {
                std::cout << "Creating WATERFALL at chunk (" << i << ", " << j << ")" << std::endl;
                waterfall = std::make_unique<Waterfall>();
                waterfall->create(Waterfall::CHUNK_SIZE * (i + 2), Waterfall::CHUNK_SIZE * (j + 2));
                worldVAO.createVBO(key, waterfall->render());  // Waterfall uses blocky render
                chunks[i].push_back(nullptr);  // Placeholder to maintain array indices
            } else {
                std::cout << "Creating SMOOTH TERRAIN at chunk (" << i << ", " << j << ")" << std::endl;
                std::unique_ptr<Chunk> chunkPtr = std::make_unique<Chunk>();
                chunks[i].push_back(std::move(chunkPtr));
                chunks[i][j]->createSmoothLandscape(chunks[i][j]->CHUNK_SIZE * (i + 2), chunks[i][j]->CHUNK_SIZE * (j + 2));
                worldVAO.createVBO(key, chunks[i][j]->renderSmooth());  // Regular terrain uses smooth render
            }
        }
    }

    std::cout << "iterated through chunks" << std::endl;

    /*
    // Create WATER PLANE underneath terrain to fill gaps - DARK BLUE water visible through cracks
    std::cout << "Creating water plane underneath terrain..." << std::endl;
    std::vector<float> waterPlane;
    int waterY = Config::World::WATER_PLANE_Y;  // From Config

    // Create single chunk-sized water plane (will be rendered at each chunk position)
    for (int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {
            // Normal pointing up
            int normX = 1, normY = 2, normZ = 1;  // (0, 1, 0) up
            int colorID = 3;  // DARK BLUE water color
            
            // Triangle 1
            int pos1 = x | (waterY << 6) | (z << 12);
            waterPlane.push_back(pos1 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

            int pos2 = (x+1) | (waterY << 6) | (z << 12);
            waterPlane.push_back(pos2 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

            int pos3 = x | (waterY << 6) | ((z+1) << 12);
            waterPlane.push_back(pos3 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

            // Triangle 2
            waterPlane.push_back(pos2 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

            int pos4 = (x+1) | (waterY << 6) | ((z+1) << 12);
            waterPlane.push_back(pos4 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

            waterPlane.push_back(pos3 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);
        }
    }
    worldVAO.createVBO("WaterPlane", waterPlane);
    std::cout << "Water plane created with " << waterPlane.size() << " vertices at y=" << waterY << std::endl;
    */
    water_plane water(view,projection,camera.position,fog);

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

        // UPDATE FLUID PHYSICS - Update particles for waterfall
        if (waterfall) {
            waterfall->updateParticles(deltaTime);
        }

        // UPDATE RAIN PHYSICS
        rain.updateParticles(deltaTime);

        // render
        // ------
        // Clear with fog color for seamless horizon blend
        glClearColor(0.7f, 0.8f, 0.9f, 1.0f);  // Match fog color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Enable Clipping Plane
        glEnable(GL_CLIP_DISTANCE0);

        // Initialize Transformation Matrices
        glm::mat4 model = glm::mat4(1.0f);
        
        // FREE CAMERA - Use keyboard/mouse controls to navigate!
        // WASD: Move, Mouse: Look around, Scroll: Zoom
        view = camera.getViewMatrix();
        float aspect = (float)Config::Rendering::WINDOW_WIDTH / Config::Rendering::WINDOW_HEIGHT;
        projection = glm::perspective(glm::radians(camera.zoom), aspect,
                                                Config::Rendering::NEAR_PLANE, Config::Rendering::FAR_PLANE);
                                                
        // Generate World
        glDisable(GL_CLIP_DISTANCE0);
        worldShader.use();
        worldShader.setFloat("time", currentFrame);  // Send time for water animation
        
        // Apply Volumetric Fog - atmospheric waterfall mist!
        fog.applyToShader(worldShader);
        glm::vec4 plane = glm::vec4(0, 0, 0, 0);
        renderWorld(worldVAO, worldShader, renderer, model, view, projection, plane);

        // RENDER FLUID PHYSICS PARTICLES
        if (waterfall) {
            std::vector<float> particleVertices = waterfall->renderParticles();
            if (!particleVertices.empty()) {
                worldVAO.createVBO("Particles", particleVertices);  // Recreate VBO each frame
                worldVAO.bindVBO("Particles");
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(7 * 20, 0.0f, -7 * 20));  // Position at waterfall chunk
                model = glm::scale(model, glm::vec3(20, 20, 20));
                worldShader.setMat4("model", model);
                renderer.draw(worldVAO, worldShader);
            }
        }

        // RENDER RAIN PARTICLES - Optimized: render each chunk's rain only once
        for (int i = 0; i < WORLD_SIZE; ++i) {
            for (int j = 0; j < WORLD_SIZE; ++j) {
                std::vector<float> rainVertices = rain.renderParticlesForChunk(i, j);
                if (!rainVertices.empty()) {
                    // Use unique VBO key per chunk to avoid recreating VBOs
                    std::string rainKey = "Rain_" + std::to_string(i) + "_" + std::to_string(j);
                    worldVAO.createVBO(rainKey, rainVertices);
                    worldVAO.bindVBO(rainKey);
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(i * 20, 0.0f, -j * 20));
                    model = glm::scale(model, glm::vec3(20, 20, 20));
                    worldShader.setMat4("model", model);
                    renderer.draw(worldVAO, worldShader);
                }
            }
        }

        water.render();

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

void renderWorld(VertexArrayWrapper &worldVAO, Shader& worldShader, Renderer& renderer, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection, glm::vec4& plane) {
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
            std::string key = "Chunk" + std::to_string(i) +","+ std::to_string(j);
            // Removed memory leak - chunk already created during initialization
            worldVAO.bindVBO(key);

            // Draw - translate THEN scale to avoid gaps between chunks
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(i * 20, 0.0f, -j * 20));
            model = glm::scale(model, glm::vec3(20, 20, 20));
            worldShader.setMat4("model", model);

            renderer.draw(worldVAO, worldShader);
        }
    }
    /*
    // Draw WATER PLANE underneath terrain - dark blue water visible through gaps
    // Render at each chunk position like terrain
    worldVAO.bindVBO("WaterPlane");
    for (int i = 0; i < WORLD_SIZE; ++i) {
        for (int j = 0; j < WORLD_SIZE; ++j) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(i * 20, 0.0f, -j * 20));
            model = glm::scale(model, glm::vec3(20, 20, 20));
            worldShader.setMat4("model", model);
            renderer.draw(worldVAO, worldShader);
        }
    }
    */
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(CamMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(CamMovement::DOWN, deltaTime);
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

    camera.processMouseMovement(xoffset, yoffset, true);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}