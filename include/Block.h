#pragma once

#include <common_types.h>

class Block {
  private:
    bool active;           // Does the block exist in the world
    BlockTexture bTexture; // Texture type of block

  public:
    /**
     * @brief Construct a new Block object
     * 
     */
    Block();

    /**
     * @brief Construct a new Block object
     * 
     * @param active       Exists in the world
     * @param blockTexture Texture type of block
     */
    Block(bool active, BlockTexture blockTexture);

    /**
     * @brief Destroy the Block object
     * 
     */
    ~Block();

    /**
     * @brief Check if block exists in the world
     * 
     * @return true  Block exists 
     * @return false Block doesn't exist (air)
     */
    bool isActive();

    /**
     * @brief Set the the block to active
     * 
     * @param status Status of block (T/F)
     */
    void setActive(bool active);

    /**
     * @brief Get the block texture
     * 
     * @return BlockTexture  The current texture
     */
    BlockTexture getTexture();

    /**
     * @brief Set the block texture
     * 
     * @param texture  The new texture
     */
    void setTexture(BlockTexture texture);
};