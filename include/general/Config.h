#pragma once

#include <glm/glm.hpp>

/**
 * @brief Centralized configuration for the entire application
 *
 * This namespace contains all magic numbers, default values, and configuration
 * parameters used throughout the project. Eliminates hardcoded values.
 */
namespace Config {

    /**
     * @brief World generation settings
     */
    namespace World {
        constexpr int SIZE = 16;              // World is 16x16 chunks
        constexpr int CHUNK_SIZE = 32;        // Each chunk is 32x32x32 blocks
        constexpr int CHUNK_WORLD_UNITS = 20; // Each chunk is 20 world units
        constexpr int WATERFALL_CHUNK_X = 7;  // Waterfall position
        constexpr int WATERFALL_CHUNK_Z = 7;
        constexpr int WATER_PLANE_Y = 8;      // Y level for underground water plane
    }

    /**
     * @brief Camera configuration
     */
    namespace Camera {
        const glm::vec3 START_POSITION(90.0f, 40.0f, -90.0f);  // Start near waterfall
        const glm::vec3 UP_VECTOR(0.0f, 1.0f, 0.0f);
        constexpr float START_YAW = -135.0f;
        constexpr float START_PITCH = -20.0f;
        constexpr float DEFAULT_ZOOM = 45.0f;
        constexpr float MOVEMENT_SPEED = 2.5f;
        constexpr float MOUSE_SENSITIVITY = 0.1f;
    }

    /**
     * @brief Lighting configuration
     */
    namespace Lighting {
        const glm::vec3 SUN_POSITION(160.0f, 150.0f, -160.0f);  // High in sky
        const glm::vec3 SUN_COLOR(1.0f, 0.95f, 0.8f);           // Warm sunlight
        constexpr float SUN_SCALE = 15.0f;                      // Visual size
    }

    /**
     * @brief Rain system configuration
     */
    namespace Rain {
        constexpr float SPAWN_HEIGHT = 80.0f;        // Y position where rain spawns
        constexpr float SPAWN_RATE = 500.0f;         // Drops per second
        constexpr float FALL_SPEED = 30.0f;          // Base falling speed
        constexpr float GROUND_LEVEL = 5.0f;         // Y level where rain despawns
        constexpr int PARTICLE_POOL_SIZE = 5000;     // Max active raindrops
        constexpr float MAX_LIFETIME = 10.0f;        // Seconds before despawn
    }

    /**
     * @brief Fog rendering configuration
     */
    namespace Fog {
        const glm::vec3 COLOR(0.7f, 0.8f, 0.9f);     // Light blue-gray
        constexpr float DENSITY = 0.02f;             // Thickness
        constexpr float START_DISTANCE = 50.0f;      // Begin at 50 units
        constexpr float END_DISTANCE = 250.0f;       // Full fog at 250 units
    }

    /**
     * @brief Rendering/Window configuration
     */
    namespace Rendering {
        constexpr int WINDOW_WIDTH = 800;
        constexpr int WINDOW_HEIGHT = 600;
        const glm::vec3 CLEAR_COLOR(0.7f, 0.8f, 0.9f);  // Match fog color
        constexpr float NEAR_PLANE = 0.1f;
        constexpr float FAR_PLANE = 1000.0f;
        constexpr float FOV = 45.0f;
    }

    /**
     * @brief Shader file paths
     */
    namespace Shaders {
        constexpr const char* WORLD_SHADER = "./shaders/WorldShader.GLSL";
        constexpr const char* LIGHT_SHADER = "./shaders/LightsShader.GLSL";
    }

    /**
     * @brief Physics constants
     */
    namespace Physics {
        constexpr float GRAVITY = -9.8f;  // m/s^2
    }
}
