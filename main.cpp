#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <GL/glew.h>
#include <iostream>
#include <mutex>
#include "../include/FFTStream.h"
#include "src/Bar.h"

constexpr unsigned int WINDOW_WIDTH = 1024;
constexpr unsigned int WINDOW_HEIGHT = 700;
constexpr float BAR_HEIGHT_SCALING = 0.005;
constexpr unsigned int NUM_BARS = 25;

std::mutex mtx;

const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "\n"
                                   "void main()\n"
                                   "{\n"
                                   "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\0";

int main() {
    // Loading song
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("../audio/raver.wav")) {
        std::cerr << "Could not load RAVER.mp3!!!" << std::endl;
        return -1;
    }

    // Creating & Starting frequency analysis stream
    float normalizedFrequencySpectrum[FFTStream::CONSIDERATION_LENGTH];
    FFTStream fftStream;
    fftStream.load(buffer);
    fftStream.setCtx(normalizedFrequencySpectrum);
    fftStream.play();

    // Setting up bars
    float bar_width = (2.0f / NUM_BARS);

    std::vector<Bar> bars;
    bars.reserve(NUM_BARS);
    for (int i = 0 ; i < NUM_BARS ; i++) {
        float x = i * bar_width - 1.0f;
        bars.push_back(Bar(x, bar_width, 0.0f));
    }

    // Creating OpenGL window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Audio Visualizer");
    glEnable(GL_TEXTURE_2D);

    // Activating the window
    window.setActive(true);

    // Initialize GLEW
    glewInit();

    // Print GLEW version
    std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

    // OpenGL stuff
    int success;
    char infoLog[512];

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int num_vertices = NUM_BARS * 12;
    int num_indices = NUM_BARS * 6;

    float vertices[] = {};

    unsigned int indices[] = {};

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, num_vertices, vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices, indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    const float frequencySpectrumToBinsScaleFactor = FFTStream::CONSIDERATION_LENGTH / NUM_BARS;

    bool running = true;
    while (running) {
        // Update the vertex data

        // Resetting bar height of all bars to 0
        for (int i = 0 ; i < NUM_BARS ; i++) {
            bars.at(i).height = 0;
        }

        // Calculating bar heights
        for (float i = 0.0f ; i < FFTStream::CONSIDERATION_LENGTH - 13 ; i++) { // todo this is ass code
            int bar_index = floor(i / frequencySpectrumToBinsScaleFactor);

            float frequency_mag = abs(normalizedFrequencySpectrum[int(i)]) * BAR_HEIGHT_SCALING;

            bars.at(bar_index).height += frequency_mag;
        }

        // todo: do not allocate these (heap memory) inside the loop
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        int vert_index = 0;
        for (Bar bar : bars) {
            bar.generate2DVertices(vertices, indices, vert_index);
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // end the program
                running = false;
            } else if (event.type == sf::Event::Resized) {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw...
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    return 0;
}