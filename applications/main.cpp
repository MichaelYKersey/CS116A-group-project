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
#include <terrain/Chunk.h>
#include <terrain/Waterfall.h>
#include <general/app_util.hpp>

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

// Create Camera - Start at waterfall with nice viewing angle
// Waterfall is at chunk (7,7) = position (140, 0, -140)
Camera camera(glm::vec3(90.0f, 40.0f, -90.0f), cameraUp, -135.0f, -20.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Light (Sun position - high in the sky!)
glm::vec3 lightPos(160.0f, 150.0f, -160.0f);  // High above waterfall, like a sun
glm::vec3 lightColor(1.0f, 0.95f, 0.8f);      // Warm sunlight color

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

    // Create Volumetric Fog system
    VolumetricFog fog;

    // Set up world
    std::vector<std::vector<std::unique_ptr<Chunk>>> chunks(WORLD_SIZE);
    std::unique_ptr<Waterfall> waterfall;  // Waterfall at chunk (7,7)

    // Iterate through chunk grid
    for (int i = 0; i < WORLD_SIZE; i++) {
        for (int j = 0; j < WORLD_SIZE; j++) {
            std::string key = "Chunk" + std::to_string(i) +","+ std::to_string(j);

            // Create waterfall in center chunk, SMOOTH landscape everywhere else
            if (i == 7 && j == 7) {
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

    // Create WATER PLANE underneath terrain to fill gaps - DARK BLUE water visible through cracks
    std::cout << "Creating water plane underneath terrain..." << std::endl;
    std::vector<float> waterPlane;
    int waterY = 8;  // Y position in chunk coordinates - closer to surface (sand level ~y=10)

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

        // UPDATE FLUID PHYSICS - Update particles for waterfall
        if (waterfall) {
            waterfall->updateParticles(deltaTime);
        }

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
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 1000.0f);
        
        // Render SUN - big glowing sphere in the sky!
        lightShader.use();
        lightVAO.bind();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(15.0f));  // BIG sun!
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightVAO.bindVBO("Light");
        renderer.draw(lightVAO, lightShader);

        // Generate World
        glDisable(GL_CLIP_DISTANCE0);
        worldShader.use();
        worldShader.setFloat("time", currentFrame);  // Send time for water animation

        // Apply Volumetric Fog - atmospheric waterfall mist!
        fog.applyToShader(worldShader);

        renderWorld(worldVAO, worldShader, renderer, model, view, projection, glm::vec4(0, 0, 0, 0));

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

    camera.processMouseMovement(xoffset, yoffset, true);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}