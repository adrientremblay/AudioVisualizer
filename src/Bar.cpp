//
// Created by adrien on 28/03/23.
//

#include "Bar.h"

Bar::Bar(float x, float height) : x(x), height(height) {

}

void Bar::generate2DVertices(std::vector<float> vertices) {
    // First Triangle
    vertices.push_back(x);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    vertices.push_back(x);
    vertices.push_back(height);
    vertices.push_back(0);

    vertices.push_back(x + bar_width);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    // Second Triangle
    vertices.push_back(x);
    vertices.push_back(height);
    vertices.push_back(0);

    vertices.push_back(x + bar_width);
    vertices.push_back(-1.0f);
    vertices.push_back(0);

    vertices.push_back(x + bar_width);
    vertices.push_back(height + bar_width);
    vertices.push_back(0);
}
