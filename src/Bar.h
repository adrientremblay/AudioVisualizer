//
// Created by adrien on 28/03/23.
//

#ifndef AUDIOVISUALIZER_BAR_H
#define AUDIOVISUALIZER_BAR_H

#include <vector>

class Bar {
public:
    float width;
    float height;
    float x;

    Bar(float x, float width, float height);

    void generate2DVertices(std::vector<float>& vertices);
};


#endif //AUDIOVISUALIZER_BAR_H