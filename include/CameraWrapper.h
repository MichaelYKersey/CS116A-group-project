#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include <include/common_types.h>

class Camera {
  private:
    /**
     * @brief Calculates the front vector from Camera's Euler Angles
     * 
     */
    void updateCameraVectors();
    
  public:
    /**
     * @brief Construct a new Camera object with vectors
     * 
     * @param position Camera position
     * @param up       Camera vector that points upwards (in world space)
     * @param yaw      Camera yaw value (left and right movement)
     * @param pitch    Camera pitch value (up and down movement)
     */
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    /**
     * @brief Construct a new Camera object with scalar
     * 
     * @param positionX Camera x position
     * @param positionY Camera y position
     * @param positionZ Camera z position
     * @param upX       Camera up x position
     * @param upY       Camera up y position 
     * @param upZ       Camera up z position
     * @param yaw       Camera yaw value (left and right movement)
     * @param pitch     Camera pitch value (up and down movement)
     */
    Camera(float positionX, float positionY, float positionZ, float upX, float upY, float upZ, float yaw, float pitch);

    /**
     * @brief Destroy the Camera object
     * 
     */
    ~Camera();

    /**
     * @brief Get the View Matrix object
     * 
     * @return glm::mat4  The view matrix calculated using Euler Angles
     */
    glm::mat4 getViewMatrix();

    /**
     * @brief Process input received from keyboard
     * 
     * @param dir       Camera direction
     * @param deltaTime Time elapsed
     */
    void processKeyboard(CamMovement dir, float deltaTime);

    /**
     * @brief Process mouse movement
     * 
     * @param offsetX        X offset
     * @param offsetY        Y offset
     * @param constrainPitch Clamp pitch angle
     */
    void processMouseMovement(float offsetX, float offsetY, GLboolean constrainPitch = true);

    /**
     * @brief Process mouse scroll
     * 
     * @param offsetY Y offset
     */
    void processMouseScroll(float offsetY);

    /**
     * @brief Invert the pitch
     * 
     */
    void invertPitch();

    // Camera properties
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 worldUp;

    // Euler angles + Camera options
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
};