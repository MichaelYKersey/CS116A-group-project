#pragma once

#include <vector>

/**
 * @brief Reusable geometry meshes for rendering
 *
 * This namespace contains standard mesh definitions used throughout
 * the rendering pipeline. Separates geometry from terrain logic.
 */
namespace Geometry {
    /**
     * @brief Standard cube mesh with normals
     *
     * Format: pos.x, pos.y, pos.z, normal.x, normal.y, normal.z
     * 36 vertices (6 faces × 2 triangles × 3 vertices)
     */
    extern const std::vector<float> CUBE_MESH;
}
