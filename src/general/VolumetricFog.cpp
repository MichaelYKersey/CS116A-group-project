#include <general/VolumetricFog.h>
#include <general/Config.h>

VolumetricFog::VolumetricFog() {
    // Initialize from Config
    fogColor = Config::Fog::COLOR;
    fogDensity = Config::Fog::DENSITY;
    fogStart = Config::Fog::START_DISTANCE;
    fogEnd = Config::Fog::END_DISTANCE;
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
