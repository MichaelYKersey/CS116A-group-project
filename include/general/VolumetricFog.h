#pragma once

#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

/**
 * @brief Volumetric Fog system for atmospheric effects
 *
 * Manages fog parameters and provides interface to apply fog to shaders.
 * Creates atmospheric depth and distance-based visibility effects.
 */
class VolumetricFog {
private:
    glm::vec3 fogColor;    // Color of fog (light blue-gray)
    float fogDensity;      // How thick the fog is
    float fogStart;        // Distance where fog begins
    float fogEnd;          // Distance where fog is maximum

public:
    /**
     * @brief Construct volumetric fog with default atmospheric parameters
     */
    VolumetricFog();

    /**
     * @brief Construct volumetric fog with custom parameters
     *
     * @param color Fog color (typically light blue-gray)
     * @param density Fog thickness (0.0 = none, 1.0 = very thick)
     * @param start Distance where fog begins
     * @param end Distance where fog reaches maximum
     */
    VolumetricFog(glm::vec3 color, float density, float start, float end);

    /**
     * @brief Apply fog parameters to shader uniforms
     *
     * @param shader Shader to apply fog parameters to
     */
    void applyToShader(Shader& shader);

    /**
     * @brief Set fog color
     *
     * @param color New fog color
     */
    void setColor(glm::vec3 color) { fogColor = color; }

    /**
     * @brief Set fog density
     *
     * @param density New fog density (0.0 to 1.0)
     */
    void setDensity(float density) { fogDensity = density; }

    /**
     * @brief Set fog distance range
     *
     * @param start Distance where fog begins
     * @param end Distance where fog is maximum
     */
    void setRange(float start, float end) { fogStart = start; fogEnd = end; }

    // Getters
    glm::vec3 getColor() const { return fogColor; }
    float getDensity() const { return fogDensity; }
    float getStart() const { return fogStart; }
    float getEnd() const { return fogEnd; }
};
