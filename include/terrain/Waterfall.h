#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <vector>

#include <terrain/Chunk.h>

// Water particle structure for fluid physics simulation
struct WaterParticle {
    glm::vec3 position;    // Current position in world space
    glm::vec3 velocity;    // Current velocity (movement per second)
    float lifetime;        // Time remaining before despawn (seconds)
    bool active;           // Is this particle alive?

    WaterParticle() : position(0.0f), velocity(0.0f), lifetime(0.0f), active(false) {}
};

class Waterfall {
private:
    Chunk terrainChunk;                           // Terrain generation (composition)

    // FLUID PHYSICS - Particle system
    std::vector<WaterParticle> waterParticles;    // Pool of water particles
    float particleSpawnTimer;                     // Time accumulator for spawning
    std::mt19937 randomGen;                       // Random number generator

    // Private helper methods for terrain generation
    void createMountain();
    void createLake();
    void carveWaterfallChannel();
    void createPool();
    void spawnParticles(float dt);

public:
    /**
     * @brief Construct a new Waterfall object
     */
    Waterfall();

    /**
     * @brief Destroy the Waterfall object
     */
    ~Waterfall();

    /**
     * @brief Create waterfall landscape with mountain, lake, channel, and pool
     *
     * @param dx Starting x coordinate for Perlin noise
     * @param dy Starting y coordinate for Perlin noise
     */
    void create(double dx, double dy);

    /**
     * @brief Create a tall tower to demonstrate shadows
     *
     * @param baseX Tower base X position
     * @param baseZ Tower base Z position
     * @param height Tower height
     */
    void createTower(int baseX, int baseZ, int height);

    /**
     * @brief Update fluid physics particles
     *
     * @param dt Delta time in seconds
     */
    void updateParticles(float dt);

    /**
     * @brief Create mesh for visible blocks
     *
     * @return std::vector<float> Vertex data
     */
    std::vector<float> render();

    /**
     * @brief Render water particles as small cubes
     *
     * @return std::vector<float> Vertex data for particles
     */
    std::vector<float> renderParticles();

    static const int CHUNK_SIZE = 32;
};
