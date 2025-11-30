#include <terrain/Chunk.h>
#include <algorithm>  // For std::max
#include <iostream>   // For logging

/* -------------------------------------------------------------------------- */
/*                                  Cube Mesh                                 */
/* -------------------------------------------------------------------------- */

// pos.x, pos.y, pos.z, normal.x, normal.y, normal.z
std::vector<float> cube = {
  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
   0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
   0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
   0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
   0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
   0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
   0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  1.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f,

  -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

  -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
   0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
   0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
   0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
  -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

  -0.5f,  0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
};

/* -------------------------------------------------------------------------- */
/*                                Block Mapping                               */
/* -------------------------------------------------------------------------- */

std::map<BlockTexture, glm::vec3> textureMap = {
  {BlockTexture::DEFAULT,        glm::vec3(0.04f,   0.44f,   0.15f)},
  {BlockTexture::GRASS,          glm::vec3(0.04f,   0.44f,   0.15f)},
  {BlockTexture::SAND,           glm::vec3(0.761f,  0.698f,  0.502f)},
  {BlockTexture::STONE,          glm::vec3(0.5725f, 0.5569f, 0.5216f)},
  {BlockTexture::ICE,            glm::vec3(0.2549f, 0.9608f, 0.9647f)},
  {BlockTexture::SNOW,           glm::vec3(1.0f,    1.0f,    1.0f)},
  {BlockTexture::WATER,          glm::vec3(0.2f,    0.5f,    0.9f)},     // Dark blue water
  {BlockTexture::WATER_PARTICLE, glm::vec3(0.4f,    0.85f,   1.0f)},    // Light cyan particles!
};

std::map<BlockTexture, int> textureID = {
  {BlockTexture::SAND,           0},
  {BlockTexture::GRASS,          1},
  {BlockTexture::STONE,          2},
  {BlockTexture::SNOW,           2},  // Snow and stone both gray - that's OK
  {BlockTexture::WATER,          3},  // Dark blue water (static blocks)
  {BlockTexture::WATER_PARTICLE, 4},  // Light cyan particles (fluid physics!)
};

Chunk::Chunk() {
  block3D = new Block **[CHUNK_SIZE];

  for (int i = 0; i < CHUNK_SIZE; i++) {
    block3D[i] = new Block *[CHUNK_SIZE];
    for (int j = 0; j < CHUNK_SIZE; j++) {
      block3D[i][j] = new Block[CHUNK_SIZE];
    }
  }

  setupHeightMap();

  // Initialize particle system
  waterParticles.reserve(2000);  // Pre-allocate for ~2000 particles
  particleSpawnTimer = 0.0f;
  randomGen = std::mt19937(std::random_device{}());
  isWaterfallChunk = false;  // Will be set to true for waterfall chunk
}

Chunk::~Chunk() {
  for (int i = 0; i < CHUNK_SIZE; ++i) {
    for (int j = 0; j < CHUNK_SIZE; ++j) {
      delete[] block3D[i][j];
    }
    delete[] block3D[i];
  }
  delete[] block3D;
}

void Chunk::update(float dt) {
  // Todo
}

void Chunk::setupHeightMap() {
  // 1. Setup Perlin Noise module
  myModule.SetFrequency(0.01f);
  heightMapBuilder.SetSourceModule(myModule);
  heightMapBuilder.SetDestNoiseMap(heightMap);
  heightMapBuilder.SetDestSize(CHUNK_SIZE, CHUNK_SIZE);

  // 2. Setup Image renderer
  renderer.SetSourceNoiseMap(heightMap);
  renderer.SetDestImage(image);
  renderer.ClearGradient();
  renderer.AddGradientPoint(-1.0000, utils::Color(0,   0,   128, 255)); // depth
  renderer.AddGradientPoint(-0.2500, utils::Color(0,   0,   255, 255)); // shallow
  renderer.AddGradientPoint( 0.0000, utils::Color(0,   128, 255, 255)); // shore
  renderer.AddGradientPoint( 0.0625, utils::Color(240, 240, 64,  255)); // sand
  renderer.AddGradientPoint( 0.1250, utils::Color(32,  160, 0,   255)); // grass
  renderer.AddGradientPoint( 0.3750, utils::Color(224, 224, 0,   255)); // dirt
  renderer.AddGradientPoint( 0.7500, utils::Color(128, 128, 128, 255)); // rock
  renderer.AddGradientPoint( 1.0000, utils::Color(255, 255, 255, 255)); // snow
  
  writer.SetSourceImage(image);
  writer.SetDestFilename("terrain.bmp");
}

BlockTexture Chunk::getTextureFromHeight(int height) {
  if (height <= 3 * CHUNK_SIZE / 10) {
    return SAND;
  }
  else if (height <= 5 * CHUNK_SIZE / 10) {
    return GRASS;
  }
  else if (height <= 9 * CHUNK_SIZE / 10) {
    return STONE;
  }

  return SNOW;
}

void Chunk::createSphere() {
  for (int z = 0; z < CHUNK_SIZE; ++z) {
    for (int y = 0; y < CHUNK_SIZE; ++y) {
      for (int x = 0; x < CHUNK_SIZE; ++x) {
        if (sqrt((float)(x - CHUNK_SIZE / 2) * (x - CHUNK_SIZE / 2) + (y - CHUNK_SIZE / 2) * (y - CHUNK_SIZE / 2) * (z - CHUNK_SIZE / 2) * (z - CHUNK_SIZE / 2)) <= CHUNK_SIZE / 2) {
          block3D[x][y][z].setActive(true);
          block3D[x][y][z].setTexture(GRASS);
        }
      }
    }
  }
}

void Chunk::createCube() {
  for (int z = 0; z < CHUNK_SIZE; ++z) {
    for (int y = 0; y < CHUNK_SIZE; ++y) {
      for (int x = 0; x < CHUNK_SIZE; ++x) {
        block3D[x][y][z].setActive(true);
        block3D[x][y][z].setTexture(GRASS);
      }
    }
  }
}

void Chunk::createCube(std::vector<float> &vertices, Block block, glm::vec3 coordinate) {
  // 1. Loop through cube mesh
  for (int i = 0; i < cube.size(); i += 6) {
    // Get vertex pos in chunk coords
    int x = (cube[i] + 0.5 + coordinate.x);
    int y = (cube[i + 1] + 0.5 + coordinate.y);
    int z = (cube[i + 2] + 0.5 + coordinate.z);

    // Get normal
    int normX = (cube[i + 3] == -1.0 ? 2 : cube[i + 3]);
    int normY = (cube[i + 4] == -1.0 ? 2 : cube[i + 4]);
    int normZ = (cube[i + 5] == -1.0 ? 2 : cube[i + 5]);

    // Bit encoding
    int position = x | y << 6 | z << 12;
    int normal = normX | normY << 2 | normZ << 4;
    int color = textureID[block.getTexture()];

    int vertex = position | normal << 18 | color << 24;

    vertices.push_back(vertex);
  }
}

void Chunk::createLandscape(double dx, double dy) {
  std::cout << "[TERRAIN] Creating landscape at offset (" << dx << ", " << dy << ")" << std::endl;

  // 1. Setup height map
  heightMapBuilder.SetBounds(dx, dx + CHUNK_SIZE - 1, dy, dy + CHUNK_SIZE - 1);
  heightMapBuilder.Build();
  renderer.Render();

  std::cout << "[TERRAIN] Height map built, generating terrain..." << std::endl;

  // 2. Loop over chunk horizontal positions
  int blockCount = 0;
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {
      // 3. Get height
      float height = std::min((float)CHUNK_SIZE, ((heightMap.GetValue(x, CHUNK_SIZE - 1 - z) + 1.0f) * (CHUNK_SIZE / 2.0f) * 1.0f));
      height = std::max(1.0f,height);
      for (int y = 0; y < height; y++) {
        block3D[x][y][z].setActive(true);
        block3D[x][y][z].setTexture(getTextureFromHeight((int)y));
        blockCount++;
      }
    }
  }

  std::cout << "[TERRAIN] Created " << blockCount << " blocks in landscape" << std::endl;
}

void Chunk::createSmoothLandscape(double dx, double dy) {
  std::cout << "[SMOOTH TERRAIN] Creating smooth landscape at offset (" << dx << ", " << dy << ")" << std::endl;

  // 1. Setup and build height map
  heightMapBuilder.SetBounds(dx, dx + CHUNK_SIZE - 1, dy, dy + CHUNK_SIZE - 1);
  heightMapBuilder.Build();
  renderer.Render();

  // 2. Store normalized height values
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {
      float rawHeight = heightMap.GetValue(x, CHUNK_SIZE - 1 - z);
      // Normalize to 0-CHUNK_SIZE range
      heightMapData[x][z] = std::min((float)CHUNK_SIZE, ((rawHeight + 1.0f) * (CHUNK_SIZE / 2.0f)));
      heightMapData[x][z] = std::max(1.0f, heightMapData[x][z]);
    }
  }

  // 3. Still create block data for non-terrain features (like waterfall)
  // But we'll render the smooth surface instead of cubes
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {
      int height = (int)heightMapData[x][z];
      for (int y = 0; y < height; y++) {
        block3D[x][y][z].setActive(true);
        block3D[x][y][z].setTexture(getTextureFromHeight((int)y));
      }
    }
  }

  std::cout << "[SMOOTH TERRAIN] Height map stored for smooth rendering" << std::endl;
}

void Chunk::clear() {
  for (int z = 0; z < CHUNK_SIZE; ++z) {
    for (int y = 0; y < CHUNK_SIZE; ++y) {
      for (int x = 0; x < CHUNK_SIZE; ++x) {
        block3D[x][y][z].setActive(false);
      }
    }
  }
}

bool Chunk::isHidden(int x, int y, int z) const {
  int count = 0;

  if (x > 0 && block3D[x - 1][y][z].isActive()) count++;
  if (x < CHUNK_SIZE - 1 && block3D[x + 1][y][z].isActive()) count++;

  if (y > 0 && block3D[x][y - 1][z].isActive()) count++;
  if (y < CHUNK_SIZE - 1 && block3D[x][y + 1][z].isActive()) count++;

  if (z > 0 && block3D[x][y][z - 1].isActive()) count++;
  if (z < CHUNK_SIZE - 1 && block3D[x][y][z + 1].isActive()) count++;

  return (count == 6);
}

std::vector<float> Chunk::render() {
  // 1. Initialize VAO
  std::vector<float> vertices;

  // 2. Get the vertices
  for (int x = 0; x < CHUNK_SIZE; ++x) {
    for (int z = 0; z < CHUNK_SIZE; ++z) {
      for (int y = 0; y < CHUNK_SIZE; ++y) {
        // Check if block is active
        if (block3D[x][y][z].isActive()) {
          if (isHidden(x, y, z)) continue;

          glm::vec3 coord = glm::vec3(x, y, z);
          createCube(vertices, block3D[x][y][z], coord);
        }
      }
    }
  }

  return vertices;
}

std::vector<float> Chunk::renderSmooth() {
  std::cout << "[SMOOTH RENDER] Generating smooth terrain mesh..." << std::endl;
  std::vector<float> vertices;

  // Create a smooth triangulated surface based on height map
  for (int x = 0; x < CHUNK_SIZE - 1; x++) {
    for (int z = 0; z < CHUNK_SIZE - 1; z++) {
      // Get heights of 4 corners of this quad
      float h00 = heightMapData[x][z];       // bottom-left
      float h10 = heightMapData[x+1][z];     // bottom-right
      float h01 = heightMapData[x][z+1];     // top-left
      float h11 = heightMapData[x+1][z+1];   // top-right

      // Calculate smooth normal using cross product of edges
      glm::vec3 v1(1.0f, h10 - h00, 0.0f);  // Edge to right
      glm::vec3 v2(0.0f, h01 - h00, 1.0f);  // Edge forward
      glm::vec3 normal = glm::normalize(glm::cross(v2, v1));

      // Convert normal to encoded format (0, 1, 2 for -1, 0, 1)
      int normX = (int)round(normal.x) + 1;
      int normY = (int)round(normal.y) + 1;
      int normZ = (int)round(normal.z) + 1;

      // Get color based on average height
      float avgHeight = (h00 + h10 + h01 + h11) / 4.0f;
      BlockTexture texture = getTextureFromHeight((int)avgHeight);
      int colorID = textureID[texture];

      // Create 2 triangles for this quad
      // Triangle 1: h00, h10, h01
      {
        // Vertex 1: (x, h00, z)
        int pos1 = x | ((int)h00 << 6) | (z << 12);
        int encoded1 = pos1 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24;
        vertices.push_back(encoded1);

        // Vertex 2: (x+1, h10, z)
        int pos2 = (x+1) | ((int)h10 << 6) | (z << 12);
        int encoded2 = pos2 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24;
        vertices.push_back(encoded2);

        // Vertex 3: (x, h01, z+1)
        int pos3 = x | ((int)h01 << 6) | ((z+1) << 12);
        int encoded3 = pos3 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24;
        vertices.push_back(encoded3);
      }

      // Triangle 2: h10, h11, h01
      {
        // Vertex 1: (x+1, h10, z)
        int pos1 = (x+1) | ((int)h10 << 6) | (z << 12);
        int encoded1 = pos1 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24;
        vertices.push_back(encoded1);

        // Vertex 2: (x+1, h11, z+1)
        int pos2 = (x+1) | ((int)h11 << 6) | ((z+1) << 12);
        int encoded2 = pos2 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24;
        vertices.push_back(encoded2);

        // Vertex 3: (x, h01, z+1)
        int pos3 = x | ((int)h01 << 6) | ((z+1) << 12);
        int encoded3 = pos3 | (normX | normY << 2 | normZ << 4) << 18 | colorID << 24;
        vertices.push_back(encoded3);
      }
    }
  }

  std::cout << "[SMOOTH RENDER] Generated " << vertices.size() << " vertices for smooth terrain" << std::endl;
  return vertices;
}

void Chunk::createWaterfallLandscape(double dx, double dy) {
    std::cout << "[WATERFALL] ========================================" << std::endl;
    std::cout << "[WATERFALL] Creating waterfall landscape..." << std::endl;

    // Create a TALL MOUNTAIN with waterfall!
    // Clear everything first
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                block3D[x][y][z].setActive(false);
            }
        }
    }
    std::cout << "[WATERFALL] Cleared chunk" << std::endl;

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
                block3D[x][y][z].setActive(true);
                block3D[x][y][z].setTexture(BlockTexture::STONE);
                mountainBlocks++;
            }
        }
    }
    std::cout << "[WATERFALL] Built mountain with " << mountainBlocks << " stone blocks" << std::endl;

    // Create WATER LAKE at the top center of mountain
    int lakeBlocks = 0;
    for (int x = 10; x < 22; x++) {
        for (int z = 10; z < 22; z++) {
            // Add 3 layers of water at mountain top
            for (int y = 22; y < 26; y++) {
                block3D[x][y][z].setActive(true);
                block3D[x][y][z].setTexture(BlockTexture::WATER);
                lakeBlocks++;
            }
        }
    }
    std::cout << "[WATERFALL] Created lake with " << lakeBlocks << " water blocks at top" << std::endl;

    // CARVE WATERFALL PATH - vertical channel down one side
    std::cout << "[WATERFALL] Carving waterfall channel (x=10-12, z=14-17, y=0-23)" << std::endl;
    for (int z = 14; z < 18; z++) {
        for (int y = 0; y < 24; y++) {
            // Remove blocks to create waterfall channel
            block3D[10][y][z].setActive(false);
            block3D[11][y][z].setActive(false);
            block3D[12][y][z].setActive(false);
        }
    }

    // Add FALLING WATER in the channel - FILL ENTIRE CHANNEL WIDTH!
    int fallingWaterBlocks = 0;
    for (int x = 10; x <= 12; x++) {  // Fill all 3 columns of the carved channel
        for (int z = 14; z < 18; z++) {
            for (int y = 3; y < 24; y++) {  // Every block for continuous waterfall
                block3D[x][y][z].setActive(true);
                block3D[x][y][z].setTexture(BlockTexture::WATER);
                fallingWaterBlocks++;
            }
        }
    }
    std::cout << "[WATERFALL] Added " << fallingWaterBlocks << " falling water blocks" << std::endl;

    // Create POOL at bottom where water lands
    int poolBlocks = 0;
    for (int x = 6; x < 14; x++) {
        for (int z = 12; z < 20; z++) {
            for (int y = 0; y < 4; y++) {
                block3D[x][y][z].setActive(true);
                block3D[x][y][z].setTexture(BlockTexture::WATER);
                poolBlocks++;
            }
        }
    }
    std::cout << "[WATERFALL] Created pool with " << poolBlocks << " water blocks at bottom" << std::endl;
    std::cout << "[WATERFALL] Waterfall complete! Total water: " << (lakeBlocks + fallingWaterBlocks + poolBlocks) << std::endl;

    // Enable particle system for this chunk
    isWaterfallChunk = true;
    std::cout << "[WATERFALL] Particle system ENABLED for fluid physics" << std::endl;
    std::cout << "[WATERFALL] ========================================" << std::endl;
}

/* -------------------------------------------------------------------------- */
/*                          FLUID PHYSICS - Particles                         */
/* -------------------------------------------------------------------------- */

void Chunk::spawnParticles(float dt) {
    if (!isWaterfallChunk) return;  // Only spawn in waterfall chunk

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

void Chunk::updateParticles(float dt) {
    if (!isWaterfallChunk) return;  // Only update in waterfall chunk

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

std::vector<float> Chunk::renderParticles() {
    std::vector<float> vertices;

    if (!isWaterfallChunk) return vertices;

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

        // Create small cube at particle position
        // Size = 0.5 blocks (half size)
        float size = 0.5f;

        // Encode position (scale to 0-32 integer range for encoding)
        int x = (int)(localPos.x * 2.0f);  // Scale up for sub-block precision
        int y = (int)(localPos.y * 2.0f);
        int z = (int)(localPos.z * 2.0f);

        // Clamp to valid range (0-63 for 6-bit encoding)
        x = std::min(63, std::max(0, x));
        y = std::min(63, std::max(0, y));
        z = std::min(63, std::max(0, z));

        // Normal pointing up (for lighting)
        int normX = 1;  // 0
        int normY = 2;  // +1 (up)
        int normZ = 1;  // 0

        // Water color (bright cyan/white for visibility)
        int colorID = 3;  // Water ID

        // Create a visible cube for this particle (reuse createCube logic)
        // Use a temporary block at this position with LIGHTER COLOR
        Block particleBlock;
        particleBlock.setActive(true);
        particleBlock.setTexture(BlockTexture::WATER_PARTICLE);  // Light cyan!

        // Convert back to glm::vec3 for createCube
        glm::vec3 cubePos(localPos.x, localPos.y, localPos.z);

        // Add full cube mesh for this particle (makes it MUCH more visible)
        createCube(vertices, particleBlock, cubePos);
    }

    return vertices;
}