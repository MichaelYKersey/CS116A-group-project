#include <terrain/Block.h>

Block::Block() {
  this->active = false;
  this->bTexture = BlockTexture::DEFAULT;
}

Block::Block(bool active, BlockTexture blockTexture) {
  this->active = active;
  this->bTexture = blockTexture;
}

Block::~Block() {
  // Empty Destructor
}

bool Block::isActive() {
  return this->active;
}

void Block::setActive(bool active) {
  this->active = active;
}

BlockTexture Block::getTexture() {
  return this->bTexture;
}

void Block::setTexture(BlockTexture texture) {
  this->bTexture = texture;
}