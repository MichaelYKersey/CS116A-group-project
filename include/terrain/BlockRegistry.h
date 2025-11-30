#pragma once

#include <glm/glm.hpp>
#include <terrain/Block.h>
#include <map>

/**
 * @brief Central registry for block materials and rendering properties
 *
 * Singleton pattern - provides global access to block texture/color data
 * without coupling it to Chunk class. Separates rendering data from terrain logic.
 */
class BlockRegistry {
private:
    std::map<BlockTexture, glm::vec3> colorMap;
    std::map<BlockTexture, int> idMap;

    // Singleton
    BlockRegistry();
    static BlockRegistry* instance;

public:
    /**
     * @brief Get singleton instance
     */
    static BlockRegistry& getInstance();

    /**
     * @brief Get RGB color for a block type
     *
     * @param texture Block texture type
     * @return glm::vec3 RGB color (0.0-1.0 range)
     */
    glm::vec3 getColor(BlockTexture texture) const;

    /**
     * @brief Get shader color ID for a block type
     *
     * @param texture Block texture type
     * @return int Color ID for shader encoding (0-7)
     */
    int getColorID(BlockTexture texture) const;

    // Prevent copying
    BlockRegistry(const BlockRegistry&) = delete;
    BlockRegistry& operator=(const BlockRegistry&) = delete;
};
