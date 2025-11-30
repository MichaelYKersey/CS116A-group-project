#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

#include <noise/noise.h>
#include <noise/noiseutils.h>

#include <terrain/Block.h>
#include <general/Renderer.h>
#include "vector"
#include "map"

class Chunk {
  private:
    module::Perlin myModule;                      // Generates Perlin noise
    utils::NoiseMap heightMap;                    // Floating-point values of noise
    utils::NoiseMapBuilderPlane heightMapBuilder; // Fills a Noise map
    utils::RendererImage renderer;                // Converts Noise map into an image
    utils::Image image;                           // Store image into memory
    utils::WriterBMP writer;                      // File writer for BMP images

    Block*** block3D;                             // 3D array of blocks
    float heightMapData[32][32];                  // Store height values for smooth terrain

  public:
    /**
     * @brief Construct a new Chunk object
     * 
     */
    Chunk();

    /**
     * @brief Destroy the Chunk object
     * 
     */
    ~Chunk();

    /**
     * @brief Per-frame updates base on time
     * 
     * @param dt  delta time
     */
    void update(float dt);

    /**
     * @brief Configure the Perlin noise module
     * 
     */
    void setupHeightMap();

    /**
     * @brief Get the Texture From Height object
     * 
     * @param height        The height of the block in the landscape
     * @return BlockTexture The corresponding block texture at that height
     */
    BlockTexture getTextureFromHeight(int height);
    
    /**
     * @brief Create a Sphere object
     * 
     */
    void createSphere();

    /**
     * @brief Create a Cube object
     * 
     */
    void createCube();

    /**
     * @brief Create a Cube object
     * 
     * @param vertices   Store output vector
     * @param block      Block type
     * @param coordinate Position
     */
    void createCube(std::vector<float> &vertices, Block block, glm::vec3 coordinate);
     
    /**
     * @brief Create Terrain based on noise
     *
     * @param dx Starting x coordinate
     * @param dy Starting y coordinate
     */
    void createLandscape(double dx, double dy);

    /**
     * @brief Create SMOOTH terrain mesh with interpolated vertices
     *
     * @param dx Starting x coordinate
     * @param dy Starting y coordinate
     */
    void createSmoothLandscape(double dx, double dy);

    /**
     * @brief Create Waterfall landscape with cliff and water pool
     *
     * @param dx Starting x coordinate
     * @param dy Starting y coordinate
     */
    void createWaterfallLandscape(double dx, double dy);

    /**
     * @brief Clear Blocks
     * 
     */
    void clear();

    /**
     * @brief Check if block is hidden 
     * 
     * @param x      x position
     * @param y      y position
     * @param z      z position
     * @return true  Fully hidden by surrounding blocks
     * @return false Not hidden
     */
    bool isHidden(int x, int y, int z) const;

    /**
     * @brief Create mesh for visible blocks
     *
     * @return std::vector<float> Vertex data
     */
    std::vector<float> render();

    /**
     * @brief Create smooth terrain mesh using height map data
     *
     * @return std::vector<float> Vertex data for smooth terrain
     */
    std::vector<float> renderSmooth();

    static const int CHUNK_SIZE = 32;
};