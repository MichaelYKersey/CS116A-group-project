#include <general/Rain.h>
#include <terrain/Chunk.h>
#include <iostream>

Rain::Rain(int worldSize)
    : worldSize(worldSize), spawnTimer(0.0f), randomGen(std::random_device{}()) {

    // Initialize rain parameters
    spawnHeight = 80.0f;      // Spawn high in the sky (world units)
    spawnRate = 500.0f;       // 500 raindrops per second for heavy rain
    fallSpeed = 30.0f;        // Base falling speed (units/sec)
    groundLevel = 5.0f;       // Despawn near ground level

    // Pre-allocate particle pool (5000 raindrops max)
    rainDrops.resize(5000);

    std::cout << "[RAIN] Rain system initialized with " << rainDrops.size()
              << " particle pool" << std::endl;
}

void Rain::spawnRainDrops(float dt) {
    spawnTimer += dt;

    // Calculate how many drops to spawn this frame
    float dropsToSpawn = spawnRate * dt;
    int dropCount = (int)dropsToSpawn;

    // Carry over fractional drops
    spawnTimer += (dropsToSpawn - dropCount);
    if (spawnTimer >= 1.0f) {
        dropCount++;
        spawnTimer -= 1.0f;
    }

    // Random distributions for spawning across the world
    std::uniform_real_distribution<float> xDist(0.0f, worldSize * 20.0f);  // World is 16 chunks * 20 units
    std::uniform_real_distribution<float> zDist(-worldSize * 20.0f, 0.0f); // Z is negative
    std::uniform_real_distribution<float> speedVariation(-2.0f, 2.0f);

    // Spawn new raindrops
    for (int i = 0; i < dropCount; i++) {
        // Find inactive particle
        for (auto& drop : rainDrops) {
            if (!drop.active) {
                drop.active = true;
                drop.position = glm::vec3(
                    xDist(randomGen),
                    spawnHeight,
                    zDist(randomGen)
                );
                drop.velocity = glm::vec3(
                    speedVariation(randomGen) * 0.5f,  // Slight horizontal drift
                    -fallSpeed + speedVariation(randomGen),
                    speedVariation(randomGen) * 0.5f
                );
                drop.lifetime = 10.0f;  // Max 10 seconds before despawn

                // Cache chunk position for fast lookups
                drop.chunkX = (int)(drop.position.x / 20.0f);
                drop.chunkZ = (int)(-drop.position.z / 20.0f);
                break;
            }
        }
    }
}

void Rain::updateParticles(float dt) {
    const float gravity = -9.8f;  // Gravity acceleration

    for (auto& drop : rainDrops) {
        if (!drop.active) continue;

        // Apply gravity
        drop.velocity.y += gravity * dt;

        // Update position
        drop.position += drop.velocity * dt;

        // Update cached chunk position (only when it changes chunks)
        int newChunkX = (int)(drop.position.x / 20.0f);
        int newChunkZ = (int)(-drop.position.z / 20.0f);
        if (newChunkX != drop.chunkX || newChunkZ != drop.chunkZ) {
            drop.chunkX = newChunkX;
            drop.chunkZ = newChunkZ;
        }

        // Decrease lifetime
        drop.lifetime -= dt;

        // Deactivate if hit ground or timed out
        if (drop.position.y <= groundLevel || drop.lifetime <= 0.0f) {
            drop.active = false;
        }

        // Deactivate if out of world bounds
        if (drop.position.x < 0.0f || drop.position.x > worldSize * 20.0f ||
            drop.position.z > 0.0f || drop.position.z < -worldSize * 20.0f) {
            drop.active = false;
        }
    }

    // Spawn new raindrops
    spawnRainDrops(dt);
}

std::vector<float> Rain::renderParticlesForChunk(int chunkX, int chunkZ) {
    std::vector<float> vertices;

    // Render only raindrops that belong to this chunk (using cached positions)
    for (const auto& drop : rainDrops) {
        if (!drop.active) continue;

        // Fast chunk check using cached values
        if (drop.chunkX != chunkX || drop.chunkZ != chunkZ) {
            continue;
        }

        // Get world position
        float x = drop.position.x;
        float y = drop.position.y;
        float z = drop.position.z;

        // Convert to chunk-local coordinates (0-31 range)
        float localX = (x - chunkX * 20.0f) / 20.0f * 32.0f;
        float localY = y / 20.0f * 32.0f;
        float localZ = (-z - chunkZ * 20.0f) / 20.0f * 32.0f;

        // Bounds check
        if (localX < 0 || localX >= 31 || localY < 0 || localY >= 31 ||
            localZ < 0 || localZ >= 31) {
            continue;
        }

        int x1 = (int)localX;
        int y1 = (int)localY;
        int z1 = (int)localZ;

        // Normal pointing up
        int normX = 1, normY = 2, normZ = 1;  // (0, 1, 0) up
        int colorID = 4;  // Light cyan water particle color

        // Create a small quad (2 triangles) for raindrop
        // Triangle 1
        int pos1 = x1 | (y1 << 6) | (z1 << 12);
        vertices.push_back(pos1 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

        int pos2 = (x1+1) | (y1 << 6) | (z1 << 12);
        vertices.push_back(pos2 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

        int pos3 = x1 | ((y1+1) << 6) | (z1 << 12);
        vertices.push_back(pos3 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

        // Triangle 2
        vertices.push_back(pos2 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

        int pos4 = (x1+1) | ((y1+1) << 6) | (z1 << 12);
        vertices.push_back(pos4 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);

        vertices.push_back(pos3 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24);
    }

    return vertices;
}

int Rain::getActiveDropCount() const {
    int count = 0;
    for (const auto& drop : rainDrops) {
        if (drop.active) count++;
    }
    return count;
}
