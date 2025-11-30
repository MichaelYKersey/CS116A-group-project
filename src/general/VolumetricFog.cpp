#include <general/VolumetricFog.h>

VolumetricFog::VolumetricFog() {
    // Default atmospheric fog settings - light blue mist
    fogColor = glm::vec3(0.7f, 0.8f, 0.9f);  // Light blue-gray
    fogDensity = 0.02f;                       // Moderate thickness
    fogStart = 50.0f;                         // Begin at 50 units
    fogEnd = 250.0f;                          // Full fog at 250 units
}

VolumetricFog::VolumetricFog(glm::vec3 color, float density, float start, float end)
    : fogColor(color), fogDensity(density), fogStart(start), fogEnd(end) {
}

void VolumetricFog::applyToShader(Shader& shader) {
    shader.use();
    shader.setVec3("fogColor", fogColor);
    shader.setFloat("fogDensity", fogDensity);
    shader.setFloat("fogStart", fogStart);
    shader.setFloat("fogEnd", fogEnd);
}
