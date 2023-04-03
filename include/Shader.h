//
// Created by adrien on 29/01/23.
//

#ifndef LEARN_OPEN_GL_SHADER_H
#define LEARN_OPEN_GL_SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/mat4x4.hpp>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    unsigned int shaderProgram; // shader program ID

    Shader(const char* vertexPath, const char* fragmentPath);

    void use();

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec4f(const std::string &name, float f1, float f2, float f3, float f4) const;

    void setVec3f(const std::string &name, float f1, float f2, float f3) const;

    void setMat4(const std::string &name, const glm::mat4& matrix) const;
};


#endif //LEARN_OPEN_GL_SHADER_H
