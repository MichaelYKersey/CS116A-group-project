#include <general/Config.h>
#include <general/water_plane.hpp>

water_plane::water_plane(glm::mat4& p_view,glm::mat4& p_projection, glm::vec3& p_view_pos, VolumetricFog p_fog) 
    : m_view(p_view) , 
    m_projection(p_projection), 
    m_shader(Shader(Config::Shaders::WATER_PLANE_SHADER)),
    m_view_pos(p_view_pos),
    m_fog(p_fog)
{ 
    float size = 300;
    float height = -5.51;
    float vertices[] = {
        // positions
        size, height, -size,  // top right
        size, height,0,  // bottom right
        0, height,0,  // bottom left
        0, height, -size,  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
void water_plane::render()
{
    m_shader.use();
    m_fog.applyToShader(m_shader);
    float time = glfwGetTime()/20.0f;
    m_shader.setVec3("viewPos",m_view_pos);
    m_shader.setFloat("perlin_progress", time);
    m_shader.setMat4("model", glm::mat4(1));
    m_shader.setMat4("view", m_view);
    m_shader.setMat4("projection", m_projection);
    m_shader.setVec3("perlin_offset",glm::vec3(time*10,-time,time));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}