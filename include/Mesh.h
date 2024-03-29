//
// Created by adrien on 03/04/23.
//

#ifndef AUDIOVISUALIZER_MESH_H
#define AUDIOVISUALIZER_MESH_H

#include <glm/glm.hpp>
#include "string"
#include "vector"
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<Vertex> changedVertices;
    // mesh data
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader &shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};


#endif //AUDIOVISUALIZER_MESH_H
