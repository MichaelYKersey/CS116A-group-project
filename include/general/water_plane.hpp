#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "general/Renderer.h"
#include "general/VolumetricFog.h"
#include "learnopengl/shader_m.h"

class water_plane {
    VolumetricFog m_fog;
    Shader m_shader;
    Renderer m_renderer;
    glm::mat4& m_view;
    glm::vec3& m_view_pos;
    glm::mat4& m_projection;
    unsigned int VBO, VAO, EBO;
    GLFWwindow* m_window;
public:
    water_plane(glm::mat4& p_view,glm::mat4& p_projection,glm::vec3& p_view_pos, VolumetricFog p_fog);
    void render();
};