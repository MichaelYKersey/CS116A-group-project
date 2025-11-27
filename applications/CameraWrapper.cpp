#include <CameraWrapper.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
  this->position = position;
  this->worldUp = up;
  this->yaw = yaw;
  this->pitch = pitch;

  this->front = glm::vec3(0.0f, 0.0f, -1.0f);
  this->movementSpeed = SPEED;
  this->mouseSensitivity = SENSITIVITY;
  this->zoom = ZOOM;

  this->updateCameraVectors();
}

Camera::Camera(float positionX, float positionY, float positionZ, float upX, float upY, float upZ, float yaw, float pitch) {
  this->position = glm::vec3(positionX, positionY, positionZ);
  this->worldUp = glm::vec3(upX, upY, upZ);
  this->yaw = yaw;
  this->pitch = pitch;

  this->front = glm::vec3(0.0f, 0.0f, -1.0f);
  this->movementSpeed = SPEED;
  this->mouseSensitivity = SENSITIVITY;
  this->zoom = ZOOM;

  this->updateCameraVectors();
}

Camera::~Camera() {
  // Empty Destructor
}

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position, position + front, up);
}

void Camera::updateCameraVectors() {
  // 1. Compute new Front vector from yaw and pitch
  glm::vec3 direction;
  
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  this->front = glm::normalize(direction);

  // 2. Recompute right and up vector based on new front vector
  this->right = glm::normalize(glm::cross(this->front, this->worldUp));
  this->up = glm::normalize(glm::cross(this->right, this->front));
}

void Camera::processKeyboard(CamMovement dir, float deltaTime) {
  float velocity = this->movementSpeed * deltaTime;
  switch(dir) {
    case FORWARD:
      this->position += this->front * velocity;
      break;
    case BACKWARD:
      this->position -= this->front * velocity;
      break;
    case LEFT:
      this->position -= this->right * velocity;
      break;
    case RIGHT:
      this->position += this->right * velocity;
      break;
  }
}

void Camera::processMouseMovement(float offsetX, float offsetY, GLboolean constrainPitch) {
  offsetX *= this->mouseSensitivity;
  offsetY *= this->mouseSensitivity;

  this->yaw += offsetX;
  this->pitch += offsetY;

  if (constrainPitch) {
    if (this->pitch > 89.0f) pitch = 89.0f;
    if (this->pitch < -89.0f) pitch = -89.0f;
  }

  this->updateCameraVectors();
}

void Camera::processMouseScroll(float offsetY) {
  this->zoom -= (float)offsetY;
  if (this->zoom < 1.0f) this->zoom = 1.0f;
  if (this->zoom > 45.0f) this->zoom = 45.0f;
}

void Camera::invertPitch() {
  this->pitch -= this->pitch;
  this->updateCameraVectors();
}