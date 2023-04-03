//
// Created by adrien on 29/01/23.
//

#include "../include/Shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    // 1. Reading source files
    std::string vertex_shader_code;
    std::string fragment_shader_code;

    std::ifstream vertex_shader_file(vertexPath);
    std::ifstream fragment_shader_file(fragmentPath);

    if (!vertex_shader_file.is_open())
        std::cerr << "Failed to open the vertex shader!" << std::endl;
    if (!fragment_shader_file.is_open())
        std::cerr << "Failed to open the fragment shader!" << std::endl;

    vertex_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragment_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        std::stringstream vertexShaderStream, fragmentShaderStream;
        vertexShaderStream << vertex_shader_file.rdbuf();
        fragmentShaderStream << fragment_shader_file.rdbuf();

        vertex_shader_file.close();
        fragment_shader_file.close();

        vertex_shader_code = vertexShaderStream.str();
        fragment_shader_code = fragmentShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Yo... one of the shaders... like didn't load brah... I'm sorry" << std::endl;
    }

    const char* vertexShaderSource = vertex_shader_code.c_str();
    const char* fragmentShaderSource = fragment_shader_code.c_str();

    // 2. Compiling shaders
    int success;
    char infoLog[512];
    // Creating vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Creating fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // creating the shader program 1
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader); // no longer needed after linking
    glDeleteShader(fragmentShader); // no longer needed after linking
}

void Shader::use() {
    glUseProgram(shaderProgram);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setVec4f(const std::string &name, float f1, float f2, float f3, float f4) const {
    glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), f1, f2, f3, f4);
}

void Shader::setVec3f(const std::string &name, float f1, float f2, float f3) const {
    glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), f1, f2, f3);
}

void Shader::setMat4(const std::string &name, const glm::mat4& matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
