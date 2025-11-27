#include <general/VertexArrayWrapper.h>

VertexArrayWrapper::VertexArrayWrapper() {
  // 1. Generate a Vertex Array Object and store ID into vArray
  glGenVertexArrays(1, &vArray);
  // 2. Bind Vertex Array into vArray for future use
  glBindVertexArray(vArray);
  // 3. Set Vertex type to default
  this->vt = Vertex_Default;
}

VertexArrayWrapper::VertexArrayWrapper(VertexType vt) {
  // 1. Generate a Vertex Array Object and store ID into vArray
  glGenVertexArrays(1, &vArray);
  // 2. Bind Vertex Array into vArray for future use
  glBindVertexArray(vArray);
  // 3. Set Vertex type to parameter's vertex type
  this->vt = vt;
}

VertexArrayWrapper::~VertexArrayWrapper() {
  // Empty Destructor
}

void VertexArrayWrapper::bind() const {
  glBindVertexArray(vArray);
}

VertexType VertexArrayWrapper::getType() const {
  return this->vt;
}

int VertexArrayWrapper::getVertexSizeBytes() const {
  int size = sizeof(float);

  switch (vt) {
    case Vertex_Texture:
      size *= 5;
      break;
    case Vertex_RGB:
      size *= 6;
      break;
    case Vertex_Default:
      size *= 3;
      break;
    case Vertex_Normal:
      size *= 6;
      break;
    case Vertex_Normal_RGB:
      size *= 9;
      break;
    case Vertex_Water:
      size *= 2;
      break;
    case Vertex_Normal_RGB_Optimized:
      size *= 1;
      break;
  }

  return size;
}

void VertexArrayWrapper::createVBO(std::string key, std::vector<float> vertices) {
  // 1. Create Buffer object
  unsigned int VBO;
  glGenBuffers(1, &VBO);

  // 2. Bind Buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // 3. Upload Vertex data into GPU
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

  // 4. Store VBO
  VBOs[key] = VBO;
}

void VertexArrayWrapper::editVBO(std::string key, std::vector<float> vertices) {
  // 1. Get VBO based on key
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[key]);

  // 2. Edit the VBO
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
}

void VertexArrayWrapper::bindVBO(std::string key) const{
  glBindBuffer(GL_ARRAY_BUFFER, VBOs.at(key));

  switch (vt) {
    case Vertex_Texture:
      // Attribute 0 : Position
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
      glEnableVertexAttribArray(0);
      // Attribute 1 : Texture
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);
      break;
    case Vertex_RGB:
      // Attribute 0 : Position
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
      glEnableVertexAttribArray(0);
      // Attribute 1 : RGB
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);
      break;
    case Vertex_Default:
      // Attribute 0 : Position
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
      glEnableVertexAttribArray(0);
      break;
    case Vertex_Normal:
      // Attribute 0 : Position
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
      glEnableVertexAttribArray(0);
      // Attribute 1 : Normal
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);
      break;
    case Vertex_Normal_RGB:
      // Attribute 0 : Position
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
      glEnableVertexAttribArray(0);
      // Attribute 1 : Normal
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);
      // Attribute 2 : RGB
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
      glEnableVertexAttribArray(2);
      break;
    case Vertex_Water:
      // Attribute 0 : Position
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
      glEnableVertexAttribArray(0);
      break;
    case Vertex_Normal_RGB_Optimized:
      // Attribute 0 : Position + Normal + RGB
      glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (void*)0);
      glEnableVertexAttribArray(0);
      break;
  }
}

int VertexArrayWrapper::getVBOsize() const {
  int size;
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

  return size;
}