#include <terrain/Waterfall.h>
#include <algorithm>  // For std::max, std::min
#include <iostream>   // For logging

// External texture mapping from Chunk.cpp
extern std::map<BlockTexture, int> textureID;

Waterfall::Waterfall() {
    // Initialize particle system
    waterParticles.reserve(2000);  // Pre-allocate for ~2000 particles
    particleSpawnTimer = 0.0f;
    randomGen = std::mt19937(std::random_device{}());
}

Waterfall::~Waterfall() {
    // Particle vector will clean up automatically
}

void Waterfall::create(double dx, double dy) {
    std::cout << "[WATERFALL] ========================================" << std::endl;
    std::cout << "[WATERFALL] Creating waterfall landscape..." << std::endl;

    // Get access to terrain blocks
    Block*** blocks = terrainChunk.getBlocks();

    // Clear everything first
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[x][y][z].setActive(false);
            }
        }
    }
    std::cout << "[WATERFALL] Cleared chunk" << std::endl;

    // Build the terrain features
    createMountain();
    createLake();
    carveWaterfallChannel();
    createPool();

    std::cout << "[WATERFALL] Waterfall complete!" << std::endl;
    std::cout << "[WATERFALL] Particle system ENABLED for fluid physics" << std::endl;

    // Add TOWER to demonstrate shadows!
    createTower(22, 16, 25);  // Tall tower next to waterfall
    std::cout << "[WATERFALL] Added shadow demonstration tower" << std::endl;
    std::cout << "[WATERFALL] ========================================" << std::endl;
}

void Waterfall::createMountain() {
    Block*** blocks = terrainChunk.getBlocks();

    // Build TALL MOUNTAIN - stone pyramid
    int mountainBlocks = 0;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            // Height based on distance from center (pyramid shape)
            int centerX = CHUNK_SIZE / 2;
            int centerZ = CHUNK_SIZE / 2;
            int distX = abs(x - centerX);
            int distZ = abs(z - centerZ);
            int maxDist = std::max(distX, distZ);
            int height = CHUNK_SIZE - maxDist - 2;  // Pyramid gets shorter toward edges

            if (height < 3) height = 3;  // Minimum base height

            // Fill with stone up to calculated height
            for (int y = 0; y < height; y++) {
                blocks[x][y][z].setActive(true);
                blocks[x][y][z].setTexture(BlockTexture::STONE);
                mountainBlocks++;
            }
        }
    }
    std::cout << "[WATERFALL] Built mountain with " << mountainBlocks << " stone blocks" << std::endl;
}

void Waterfall::createLake() {
    Block*** blocks = terrainChunk.getBlocks();

    // Create WATER LAKE at the top center of mountain
    int lakeBlocks = 0;
    for (int x = 10; x < 22; x++) {
        for (int z = 10; z < 22; z++) {
            // Add 3 layers of water at mountain top
            for (int y = 22; y < 26; y++) {
                blocks[x][y][z].setActive(true);
                blocks[x][y][z].setTexture(BlockTexture::WATER);
                lakeBlocks++;
            }
        }
    }
    std::cout << "[WATERFALL] Created lake with " << lakeBlocks << " water blocks at top" << std::endl;
}

void Waterfall::carveWaterfallChannel() {
    Block*** blocks = terrainChunk.getBlocks();

    // CARVE WATERFALL PATH - vertical channel down one side
    std::cout << "[WATERFALL] Carving waterfall channel (x=10-12, z=14-17, y=0-23)" << std::endl;
    for (int z = 14; z < 18; z++) {
        for (int y = 0; y < 24; y++) {
            // Remove blocks to create waterfall channel
            blocks[10][y][z].setActive(false);
            blocks[11][y][z].setActive(false);
            blocks[12][y][z].setActive(false);
        }
    }

    // Add FALLING WATER in the channel - FILL ENTIRE CHANNEL WIDTH!
    int fallingWaterBlocks = 0;
    for (int x = 10; x <= 12; x++) {  // Fill all 3 columns of the carved channel
        for (int z = 14; z < 18; z++) {
            for (int y = 3; y < 24; y++) {  // Every block for continuous waterfall
                blocks[x][y][z].setActive(true);
                blocks[x][y][z].setTexture(BlockTexture::WATER);
                fallingWaterBlocks++;
            }
        }
    }
    std::cout << "[WATERFALL] Added " << fallingWaterBlocks << " falling water blocks" << std::endl;
}

void Waterfall::createPool() {
    Block*** blocks = terrainChunk.getBlocks();

    // Create POOL at bottom where water lands
    int poolBlocks = 0;
    for (int x = 6; x < 14; x++) {
        for (int z = 12; z < 20; z++) {
            for (int y = 0; y < 4; y++) {
                blocks[x][y][z].setActive(true);
                blocks[x][y][z].setTexture(BlockTexture::WATER);
                poolBlocks++;
            }
        }
    }
    std::cout << "[WATERFALL] Created pool with " << poolBlocks << " water blocks at bottom" << std::endl;
}

void Waterfall::createTower(int baseX, int baseZ, int height) {
    std::cout << "[TOWER] Building tower at (" << baseX << ", " << baseZ << ") height=" << height << std::endl;

    Block*** blocks = terrainChunk.getBlocks();

    // Build a 3x3 stone tower
    for (int x = baseX - 1; x <= baseX + 1; x++) {
        for (int z = baseZ - 1; z <= baseZ + 1; z++) {
            for (int y = 0; y < height; y++) {
                if (x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE && y < CHUNK_SIZE) {
                    blocks[x][y][z].setActive(true);
                    blocks[x][y][z].setTexture(BlockTexture::STONE);
                }
            }
        }
    }

    std::cout << "[TOWER] Tower complete - ready to cast shadows!" << std::endl;
}

/* -------------------------------------------------------------------------- */
/*                          FLUID PHYSICS - Particles                         */
/* -------------------------------------------------------------------------- */

void Waterfall::spawnParticles(float dt) {
    particleSpawnTimer += dt;

    // Spawn rate: ~60 particles per second (more dramatic!)
    float spawnInterval = 1.0f / 60.0f;

    while (particleSpawnTimer >= spawnInterval) {
        particleSpawnTimer -= spawnInterval;

        // Try to find inactive particle to reuse
        WaterParticle* particle = nullptr;
        for (auto& p : waterParticles) {
            if (!p.active) {
                particle = &p;
                break;
            }
        }

        // If no inactive particle, create new one
        if (!particle) {
            waterParticles.emplace_back();
            particle = &waterParticles.back();
        }

        // SPAWN FROM ALL 4 SIDES OF MOUNTAIN!
        std::uniform_real_distribution<float> randVel(-0.8f, 0.8f);
        std::uniform_int_distribution<int> sideChoice(0, 3);  // Pick which side: 0=North, 1=South, 2=East, 3=West

        int side = sideChoice(randomGen);

        // Lake is at x=10-22, z=10-22, y=22-26
        // Mountain center is at x=16, z=16

        if (side == 0) {
            // NORTH SIDE (positive Z edge)
            std::uniform_real_distribution<float> distX(11.0f, 21.0f);
            particle->position = glm::vec3(distX(randomGen), 24.0f, 21.5f);
            particle->velocity = glm::vec3(randVel(randomGen) * 0.5f, -0.5f, 3.0f + randVel(randomGen));  // Shoot NORTH
        }
        else if (side == 1) {
            // SOUTH SIDE (negative Z edge)
            std::uniform_real_distribution<float> distX(11.0f, 21.0f);
            particle->position = glm::vec3(distX(randomGen), 24.0f, 10.5f);
            particle->velocity = glm::vec3(randVel(randomGen) * 0.5f, -0.5f, -3.0f + randVel(randomGen));  // Shoot SOUTH
        }
        else if (side == 2) {
            // EAST SIDE (positive X edge)
            std::uniform_real_distribution<float> distZ(11.0f, 21.0f);
            particle->position = glm::vec3(21.5f, 24.0f, distZ(randomGen));
            particle->velocity = glm::vec3(3.0f + randVel(randomGen), -0.5f, randVel(randomGen) * 0.5f);  // Shoot EAST
        }
        else {
            // WEST SIDE (negative X edge) - original waterfall side
            std::uniform_real_distribution<float> distZ(11.0f, 21.0f);
            particle->position = glm::vec3(10.5f, 24.0f, distZ(randomGen));
            particle->velocity = glm::vec3(-3.0f + randVel(randomGen), -0.5f, randVel(randomGen) * 0.5f);  // Shoot WEST
        }

        particle->lifetime = 4.0f;  // Live for 4 seconds
        particle->active = true;
    }
}

void Waterfall::updateParticles(float dt) {
    int activeCount = 0;

    for (auto& particle : waterParticles) {
        if (!particle.active) continue;

        activeCount++;

        // PHYSICS: Apply gravity
        const float GRAVITY = 9.8f;  // m/s^2 downward
        particle.velocity.y -= GRAVITY * dt;

        // UPDATE: Move particle based on velocity
        particle.position += particle.velocity * dt;

        // COLLISION: Check if particle hit the ground/pool
        float poolLevel = 3.5f;  // Pool surface height
        if (particle.position.y <= poolLevel) {
            // SPLASH! Bounce with more energy for visibility
            particle.position.y = poolLevel;  // Keep above ground
            particle.velocity.y = -particle.velocity.y * 0.6f;  // Bounce (60% energy - more visible!)
            particle.velocity.x *= 0.8f;  // Less friction
            particle.velocity.z *= 0.8f;

            // If moving too slow, stop bouncing
            if (std::abs(particle.velocity.y) < 0.3f) {
                particle.velocity.y = 0.0f;
                particle.velocity.x *= 0.3f;
                particle.velocity.z *= 0.3f;
            }
        }

        // LIFETIME: Decrease and deactivate when expired
        particle.lifetime -= dt;
        if (particle.lifetime <= 0.0f) {
            particle.active = false;
        }

        // OUT OF BOUNDS: Deactivate if particle falls too far
        if (particle.position.y < -5.0f || particle.position.y > 50.0f) {
            particle.active = false;
        }
    }

    // Spawn new particles
    spawnParticles(dt);

    // Optional: Log every few seconds
    static float logTimer = 0.0f;
    logTimer += dt;
    if (logTimer > 2.0f) {
        std::cout << "[PARTICLES] Active: " << activeCount << " / " << waterParticles.size() << std::endl;
        logTimer = 0.0f;
    }
}

std::vector<float> Waterfall::render() {
    return terrainChunk.render();
}

std::vector<float> Waterfall::renderParticles() {
    std::vector<float> vertices;

    // Render each active particle as a small cube
    for (const auto& particle : waterParticles) {
        if (!particle.active) continue;

        // Particle position in chunk-local coordinates (0-32 range)
        glm::vec3 localPos = particle.position;

        // Skip if particle is outside chunk bounds
        if (localPos.x < 0 || localPos.x >= CHUNK_SIZE ||
            localPos.y < 0 || localPos.y >= CHUNK_SIZE ||
            localPos.z < 0 || localPos.z >= CHUNK_SIZE) {
            continue;
        }

        // Create a visible cube for this particle with LIGHTER COLOR
        Block particleBlock;
        particleBlock.setActive(true);
        particleBlock.setTexture(BlockTexture::WATER_PARTICLE);  // Light cyan!

        // Convert to glm::vec3 for createCube
        glm::vec3 cubePos(localPos.x, localPos.y, localPos.z);

        // Add full cube mesh for this particle (makes it MUCH more visible)
        terrainChunk.createCube(vertices, particleBlock, cubePos);
    }

    return vertices;
}
