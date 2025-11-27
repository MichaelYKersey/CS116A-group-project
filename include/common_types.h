#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vector"
#include "map"
#include <string>

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