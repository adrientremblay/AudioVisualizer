//
// Created by adrien on 04/04/23.
//

#include "../include/Model.h"

 Model::Model(char *path) {
    loadModel(path);
}

void Model::Draw(Shader &shader) {
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}