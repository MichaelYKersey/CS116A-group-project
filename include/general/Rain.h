#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <unordered_map>

/**
 * @brief Rain particle for weather simulation
 */
struct RainDrop {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;
    bool active;
    int chunkX;  // Cached chunk position for fast lookups
    int chunkZ;

    RainDrop() : position(0.0f), velocity(0.0f), lifetime(0.0f), active(false), chunkX(-1), chunkZ(-1) {}
};

/**
 * @brief Rain system for atmospheric weather effects
 *
 * Manages rainfall particles that fall from the sky across the entire world.
 * Particles spawn at high altitude and fall with gravity until hitting the ground.
 */
class Rain {
private:
    std::vector<RainDrop> rainDrops;
    float spawnTimer;
    std::mt19937 randomGen;

    // Rain parameters
    int worldSize;           // Size of the world in chunks (16x16)
    float spawnHeight;       // Height where rain spawns (in world units)
    float spawnRate;         // Raindrops per second
    float fallSpeed;         // Base falling speed
    float groundLevel;       // Y level where rain despawns

    void spawnRainDrops(float dt);

public:
    /**
     * @brief Construct rain system with default atmospheric parameters
     *
     * @param worldSize Size of the world in chunks (default 16)
     */
    Rain(int worldSize = 16);

    /**
     * @brief Update rain particle physics
     *
     * @param dt Delta time since last frame
     */
    void updateParticles(float dt);

    /**
     * @brief Render rain particles for a specific chunk
     *
     * @param chunkX Chunk X coordinate (0-15)
     * @param chunkZ Chunk Z coordinate (0-15)
     * @return Vector of encoded vertices for GPU rendering
     */
    std::vector<float> renderParticlesForChunk(int chunkX, int chunkZ);

    // Parameter setters
    void setSpawnRate(float rate) { spawnRate = rate; }
    void setFallSpeed(float speed) { fallSpeed = speed; }
    void setSpawnHeight(float height) { spawnHeight = height; }
    void setGroundLevel(float level) { groundLevel = level; }

    // Getters
    float getSpawnRate() const { return spawnRate; }
    float getFallSpeed() const { return fallSpeed; }
    int getActiveDropCount() const;
};
