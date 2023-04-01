#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <GL/glew.h>
#include <iostream>
#include <mutex>
#include "../include/FFTStream.h"
#include "src/Bar.h"
#include <chrono>
#include <ctime>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr unsigned int WINDOW_WIDTH = 1024;
constexpr unsigned int WINDOW_HEIGHT = 700;
constexpr float BAR_HEIGHT_SCALING = 0.005;
constexpr unsigned int NUM_BARS = 30;
constexpr unsigned int FRAMES_PER_SECOND = 30;
const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;

std::mutex mtx;

const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "uniform mat4 view;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = view * vec4(aPos, 1.0);\n"
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
    fftStream.setVolume(0);

    // Setting up bars
    float bar_width = (1.0f / NUM_BARS);

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

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    const float frequencySpectrumToBinsScaleFactor = FFTStream::CONSIDERATION_LENGTH / NUM_BARS;
    const float barWidth = 1.0f / NUM_BARS;

    // Camera stuff
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    // Draw Wireframes
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int mode = 2;

    float bar_vertices[] = {
            1.0f,  1.0f, 0.0f,  // top right
            1.0f, -1.0f, 0.0f,  // bottom right
            -1.0f, -1.0f, 0.0f,  // bottom left
            -1.0f,  1.0f, 0.0f   // top left
    };
    unsigned int bar_indices[] = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bar_vertices) , bar_vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bar_indices), bar_indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned long next_game_tick = std::chrono::system_clock::now().time_since_epoch().count();
    unsigned long sleep_time = 0;
    bool running = true;
    while (running) {
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

        // Drawing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        for (int bar_index = 0 ; bar_index < bars.size() ; bar_index++) {
            glm::mat4 view = glm::lookAt(cameraPos,
                                         cameraTarget,
                                         cameraUp);
            // todo: can I simplify the position calculation?
            view = glm::translate(view, glm::vec3(-1.0 + ((bar_index + 1) * bar_width * 2 - (0.5 * bar_width)), 0.0, 0.0));
            view = glm::scale(view, glm::vec3(bar_width, bars.at(bar_index).height, 1.0));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

            // draw...
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, sizeof(bar_indices), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // end the current frame (internally swaps the front and back buffers)
        window.display();

        // FPS stuff
        next_game_tick += SKIP_TICKS;
        sleep_time = next_game_tick - std::chrono::system_clock::now().time_since_epoch().count();
        if (sleep_time >= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
    }

    return 0;
}