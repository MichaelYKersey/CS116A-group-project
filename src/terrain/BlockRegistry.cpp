#include <terrain/BlockRegistry.h>

// Initialize singleton instance
BlockRegistry* BlockRegistry::instance = nullptr;

BlockRegistry::BlockRegistry() {
    // Initialize color map (RGB values for rendering)
    colorMap[BlockTexture::DEFAULT]        = glm::vec3(0.04f,   0.44f,   0.15f);
    colorMap[BlockTexture::GRASS]          = glm::vec3(0.04f,   0.44f,   0.15f);
    colorMap[BlockTexture::SAND]           = glm::vec3(0.761f,  0.698f,  0.502f);
    colorMap[BlockTexture::STONE]          = glm::vec3(0.5725f, 0.5569f, 0.5216f);
    colorMap[BlockTexture::ICE]            = glm::vec3(0.2549f, 0.9608f, 0.9647f);
    colorMap[BlockTexture::SNOW]           = glm::vec3(1.0f,    1.0f,    1.0f);
    colorMap[BlockTexture::WATER]          = glm::vec3(0.2f,    0.5f,    0.9f);     // Dark blue
    colorMap[BlockTexture::WATER_PARTICLE] = glm::vec3(0.4f,    0.85f,   1.0f);    // Light cyan

    // Initialize shader ID map (0-7 for 3-bit encoding)
    idMap[BlockTexture::SAND]           = 0;
    idMap[BlockTexture::GRASS]          = 1;
    idMap[BlockTexture::STONE]          = 2;
    idMap[BlockTexture::SNOW]           = 2;  // Snow and stone both gray
    idMap[BlockTexture::WATER]          = 3;  // Dark blue water (static blocks)
    idMap[BlockTexture::WATER_PARTICLE] = 4;  // Light cyan particles (fluid physics)
}

BlockRegistry& BlockRegistry::getInstance() {
    if (instance == nullptr) {
        instance = new BlockRegistry();
    }
    return *instance;
}

glm::vec3 BlockRegistry::getColor(BlockTexture texture) const {
    auto it = colorMap.find(texture);
    if (it != colorMap.end()) {
        return it->second;
    }
    // Default fallback
    return glm::vec3(1.0f, 0.0f, 1.0f);  // Magenta for errors
}

int BlockRegistry::getColorID(BlockTexture texture) const {
    auto it = idMap.find(texture);
    if (it != idMap.end()) {
        return it->second;
    }
    // Default fallback
    return 0;
}
