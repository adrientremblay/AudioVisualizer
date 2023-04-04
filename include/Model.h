//
// Created by adrien on 04/04/23.
//

#ifndef AUDIOVISUALIZER_MODEL_H
#define AUDIOVISUALIZER_MODEL_H

#include "Mesh.h"
#include <assimp/scene.h>

class Model {
public:
    Model(char *path);
    void Draw(Shader &shader);
private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif //AUDIOVISUALIZER_MODEL_H
