#pragma once

#include <glad/glad.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <common_types.h>

class VertexArrayWrapper {
  private:
    unsigned int vArray;
    VertexType vt;
    std::map<std::string, unsigned int> VBOs;
    
  public:
    /**
     * @brief Construct a new Vertex Buffer object
     * 
     */
    VertexArrayWrapper();

    /**
     * @brief Construct a new Vertex Buffer object
     * 
     * @param vt The vertex type passed
     */
    VertexArrayWrapper(VertexType vt);

    /**
     * @brief Destroy the Vertex Buffer object
     * 
     */
    ~VertexArrayWrapper();

    /**
     * @brief Binds to current VAO stored in object
     * 
     */
    void bind() const;

    /**
     * @brief Get the Vertex Type object
     * 
     * @return VertexType  The type of vertex
     */
    VertexType getType() const;

    /**
     * @brief Get the Vertex byte size
     * 
     * @return int   The size of the vertex based on its type
     */
    int getVertexSizeBytes() const;

    /**
     * @brief Create a new Vertex Buffer Object
     * 
     * @param key      VBO ID
     * @param vertices The vertex data to store
     */
    void createVBO(std::string key, std::vector<float> vertices);

    /**
     * @brief Edit the Vertex Buffer Object
     * 
     * @param key      VBO ID
     * @param vertices The vertices to store
     */
    void editVBO(std::string key, std::vector<float> vertices);

    /**
     * @brief Bind VBO to VAO 
     * 
     * @param key VBO ID
     */
    void bindVBO(std::string key) const;

    /**
     * @brief Get size of VBO
     * 
     * @return int  The size value
     */
    int getVBOsize() const;
};