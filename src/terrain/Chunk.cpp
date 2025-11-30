#include <terrain/Chunk.h>
#include <algorithm>  // For std::max

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
  {BlockTexture::DEFAULT, glm::vec3(0.04f,   0.44f,   0.15f)},
  {BlockTexture::GRASS,   glm::vec3(0.04f,   0.44f,   0.15f)},
  {BlockTexture::SAND,    glm::vec3(0.761f,  0.698f,  0.502f)},
  {BlockTexture::STONE,   glm::vec3(0.5725f, 0.5569f, 0.5216f)},
  {BlockTexture::ICE,     glm::vec3(0.2549f, 0.9608f, 0.9647f)},
  {BlockTexture::SNOW,    glm::vec3(1.0f,    1.0f,    1.0f)},
  {BlockTexture::WATER,   glm::vec3(0.2f,    0.5f,    0.9f)},  // Blue water
};

std::map<BlockTexture, int> textureID = {
  {BlockTexture::SAND,    0},
  {BlockTexture::GRASS,   1},
  {BlockTexture::STONE,   2},
  {BlockTexture::SNOW,    2},  // Snow and stone both gray - that's OK
  {BlockTexture::WATER,   3},  // Water gets its own ID = 3 (BLUE!)
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
  // 1. Setup height map
  heightMapBuilder.SetBounds(dx, dx + CHUNK_SIZE - 1, dy, dy + CHUNK_SIZE - 1);
  heightMapBuilder.Build();
  renderer.Render();

  // 2. Loop over chunk horizontal positions
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {
      // 3. Get height
      float height = std::min((float)CHUNK_SIZE, ((heightMap.GetValue(x, CHUNK_SIZE - 1 - z) + 1.0f) * (CHUNK_SIZE / 2.0f) * 1.0f));
      height = std::max(1.0f,height);
      for (int y = 0; y < height; y++) {
        block3D[x][y][z].setActive(true);
        block3D[x][y][z].setTexture(getTextureFromHeight((int)y));
      }
    }
  }
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

void Chunk::createWaterfallLandscape(double dx, double dy) {
    // Create a TALL MOUNTAIN with waterfall!
    // Clear everything first
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                block3D[x][y][z].setActive(false);
            }
        }
    }

    // Build TALL MOUNTAIN - stone pyramid
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
            }
        }
    }

    // Create WATER LAKE at the top center of mountain
    for (int x = 10; x < 22; x++) {
        for (int z = 10; z < 22; z++) {
            // Add 3 layers of water at mountain top
            for (int y = 22; y < 26; y++) {
                block3D[x][y][z].setActive(true);
                block3D[x][y][z].setTexture(BlockTexture::WATER);
            }
        }
    }

    // CARVE WATERFALL PATH - vertical channel down one side
    for (int z = 14; z < 18; z++) {
        for (int y = 0; y < 24; y++) {
            // Remove blocks to create waterfall channel
            block3D[10][y][z].setActive(false);
            block3D[11][y][z].setActive(false);
            block3D[12][y][z].setActive(false);
        }
    }

    // Add FALLING WATER in the channel - FILL ENTIRE CHANNEL WIDTH!
    for (int x = 10; x <= 12; x++) {  // Fill all 3 columns of the carved channel
        for (int z = 14; z < 18; z++) {
            for (int y = 3; y < 24; y++) {  // Every block for continuous waterfall
                block3D[x][y][z].setActive(true);
                block3D[x][y][z].setTexture(BlockTexture::WATER);
            }
        }
    }

    // Create POOL at bottom where water lands
    for (int x = 6; x < 14; x++) {
        for (int z = 12; z < 20; z++) {
            for (int y = 0; y < 4; y++) {
                block3D[x][y][z].setActive(true);
                block3D[x][y][z].setTexture(BlockTexture::WATER);
            }
        }
    }
}