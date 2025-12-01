#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "general/Renderer.h"
#include "general/VolumetricFog.h"
#include "learnopengl/shader_m.h"

/**
 * @brief Advanced water plane using 4D Perlin noise for realistic animated water
 *
 * This class renders a large flat water surface beneath the terrain using procedural
 * 4D Perlin noise (x, y, z, time) computed entirely on the GPU. The water features:
 * - Animated flowing patterns via time-based offsets
 * - White foam on wave crests (noise threshold detection)
 * - Volumetric fog integration for distance-based fading
 * - Layered octave noise for realistic detail
 *
 * The implementation uses a custom hash-based random number generator in the fragment
 * shader to generate smooth Perlin noise without texture lookups, demonstrating
 * advanced shader programming techniques.
 */
class water_plane {
    VolumetricFog m_fog;
    Shader m_shader;
    Renderer m_renderer;
    glm::mat4& m_view;
    glm::vec3& m_view_pos;
    glm::mat4& m_projection;
    unsigned int VBO, VAO, EBO;   // OpenGL buffer objects
    GLFWwindow* m_window;         // Window reference

public:
    /**
     * @brief Construct water plane with required rendering matrices
     *
     * @param p_view Reference to view matrix (updated per frame)
     * @param p_projection Reference to projection matrix
     * @param p_view_pos Reference to camera position (for fog calculation)
     * @param p_fog Volumetric fog instance to blend with water
     */
    water_plane(glm::mat4& p_view, glm::mat4& p_projection, glm::vec3& p_view_pos, VolumetricFog p_fog);

    /**
     * @brief Render animated water plane
     *
     * Updates time-based uniforms and renders the water quad with procedural noise.
     * The shader computes 3 octaves of 4D Perlin noise to create flowing water patterns.
     */
    void render();
};