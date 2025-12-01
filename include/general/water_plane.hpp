#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "general/Renderer.h"
#include "learnopengl/shader_m.h"

class water_plane {
    Shader m_shader;
    Renderer m_renderer;
    glm::mat4& m_view;
    glm::mat4& m_projection;
    unsigned int VBO, VAO, EBO;
    GLFWwindow* m_window;
public:
    water_plane(glm::mat4& p_view,glm::mat4& p_projection);
    void render();
};