//
// Created by adrien on 28/03/23.
//

#include "Bar.h"

Bar::Bar(float x, float width, float height) : x(x), width(width), height(height) {

}

void Bar::generate2DVertices(std::vector<float>& vertices, std::vector<unsigned int>& indices, int& vert_index) {
    // First Triangle
    indices.push_back(vert_index + 0);
    indices.push_back(vert_index + 1);
    indices.push_back(vert_index + 2);

    // Second Triangle
    indices.push_back(vert_index + 1);
    indices.push_back(vert_index + 2);
    indices.push_back(vert_index + 3);

    vert_index+=4;

    // Adding vertices
    vertices.push_back(x);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    vertices.push_back(x);
    vertices.push_back(height - 1.0f);
    vertices.push_back(0);

    vertices.push_back(x + width);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    vertices.push_back(x + width);
    vertices.push_back(height - 1.0f);
    vertices.push_back(0);
}

void Bar::generate3DVertices(std::vector<float>& vertices, std::vector<unsigned int>& indices, int& vert_index) {
    // Indices
    indices.push_back(vert_index + 0);
    indices.push_back(vert_index + 1);
    indices.push_back(vert_index + 2);

    indices.push_back(vert_index + 1);
    indices.push_back(vert_index + 2);
    indices.push_back(vert_index + 3);

    indices.push_back(vert_index + 2);
    indices.push_back(vert_index + 3);
    indices.push_back(vert_index + 6);

    indices.push_back(vert_index + 3);
    indices.push_back(vert_index + 7);
    indices.push_back(vert_index + 6);

    vert_index+=8;

    // Adding vertices
    vertices.push_back(x);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    vertices.push_back(x);
    vertices.push_back(height - 1.0f);
    vertices.push_back(0);

    vertices.push_back(x + width);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    vertices.push_back(x + width);
    vertices.push_back(height - 1.0f);
    vertices.push_back(0);

    vertices.push_back(x);
    vertices.push_back(-1.0f);
    vertices.push_back(0.5f);

    vertices.push_back(x);
    vertices.push_back(height - 1.0f);
    vertices.push_back(0.5f);

    vertices.push_back(x + width);
    vertices.push_back(-1.0f);
    vertices.push_back(0.5f);

    vertices.push_back(x + width);
    vertices.push_back(height - 1.0f);
    vertices.push_back(0.5f);
}
