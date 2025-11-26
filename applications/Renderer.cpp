#include "include/Renderer.h"

void Renderer::draw(const VertexArrayWrapper& vao, Shader& shader) const {
  vao.bind();
  shader.use();
  int size = vao.getVBOsize() / vao.getVertexSizeBytes();
  glDrawArrays(GL_TRIANGLES, 0, size);
}