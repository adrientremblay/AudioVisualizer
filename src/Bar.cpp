//
// Created by adrien on 28/03/23.
//

#include "Bar.h"

Bar::Bar(float x, float width, float height) : x(x), width(width), height(height) {

}

void Bar::generate2DVertices(std::vector<float>& vertices) {
    // First Triangle
    vertices.push_back(x);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    vertices.push_back(x);
    vertices.push_back(height - 1.0f);
    vertices.push_back(0);

    vertices.push_back(x + width);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    // Second Triangle
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
