#pragma once

#include "VertexArrayWrapper.h"
#include "learnopengl/shader_m.h"

class Renderer {
  public:
    /**
     * @brief Draws triangles
     * 
     * @param vao     The VAO to use
     * @param shader  The Shader to use
     */
    virtual void draw(const VertexArrayWrapper& vao, Shader& shader) const;
};