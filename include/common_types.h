#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vector"
#include "map"

/**
 * @brief Types of Vertex Structure
 * 
 */
enum VertexType {
  Vertex_Texture,               // x, y, z, tx, ty
  Vertex_RGB,                   // x, y, z, r, g, b
  Vertex_Default,               // x, y, z
  Vertex_Normal,                // x, y, z, n1, n2, n3
  Vertex_Normal_RGB,            // x, y, z, n1, n2, n3, r, g, b
  Vertex_Water,                 // x, z
  Vertex_Normal_RGB_Optimized,  // position | normal | color
};

/**
 * @brief Types of Camera Movements
 * 
 */
enum CamMovement {
  FORWARD, 
  BACKWARD,
  LEFT,
  RIGHT,  
};

/**
 * @brief Store Vertex/Fragment source
 * 
 */
struct Shaders {
  std::string vertex;
  std::string fragment;
};

/**
 * @brief Types of Shaders
 * 
 */
enum class ShaderType {
  NONE     = -1,
  VERTEX   = 0,
  FRAGMENT = 1,
};

/**
 * @brief Types of Blocks
 * 
 */
enum BlockTexture {
  DEFAULT,
  GRASS,
  DIRT,
  WATER,
  STONE,
  WOOD,
  SAND,
  ICE,
  SNOW, 
  TOTAL,
};

/* -------------------------------------------------------------------------- */
/*                            Default Camera Values                           */
/* -------------------------------------------------------------------------- */
const float YAW         = -90.0f;
const float PITCH       = 0.0f;
const float SPEED       = 50.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM        = 45.0f;

/* -------------------------------------------------------------------------- */
/*                                  Cube Mesh                                 */
/* -------------------------------------------------------------------------- */

// pos.x, pos.y, pos.z, normal.x, normal.y, normal.z
std::vector<float> cube = {
  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
   0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
   0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
   0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
   0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
   0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
   0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f,

  -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

  -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
   0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
   0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
   0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
  -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

  -0.5f,  0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
};

/* -------------------------------------------------------------------------- */
/*                                Block Mapping                               */
/* -------------------------------------------------------------------------- */

std::map<BlockTexture, glm::vec3> textureMap = {
  {BlockTexture::DEFAULT, glm::vec3(0.04f,   0.44f,   0.15f)},
  {BlockTexture::GRASS,   glm::vec3(0.04f,   0.44f,   0.15f)},
  {BlockTexture::SAND,    glm::vec3(0.761f,  0.698f,  0.502f)},
  {BlockTexture::STONE,   glm::vec3(0.5725f, 0.5569f, 0.5216f)},
  {BlockTexture::ICE,     glm::vec3(0.2549f, 0.9608f, 0.9647f)},
  {BlockTexture::SNOW,    glm::vec3(1.0f,    1.0f,    1.0f)},
};

std::map<BlockTexture, int> textureID = {
  {BlockTexture::SAND,    0},
  {BlockTexture::GRASS,   1},
  {BlockTexture::STONE,   2},
  {BlockTexture::SNOW,    3},
};