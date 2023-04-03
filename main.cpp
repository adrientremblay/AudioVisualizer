#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <GL/glew.h>
#include <iostream>
#include <mutex>
#include "../include/FFTStream.h"
#include <chrono>
#include <ctime>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "include/Shader.h"

constexpr unsigned int WINDOW_WIDTH = 1024;
constexpr unsigned int WINDOW_HEIGHT = 700;
constexpr float BAR_HEIGHT_SCALING = 0.005;
constexpr unsigned int NUM_BARS = 30;
constexpr unsigned int FRAMES_PER_SECOND = 30;
const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;

std::mutex mtx;

enum Mode {
    TWO_DIMENSIONAL,
    TWO_DIMENSIONAL_SPINNING,
    THREE_DIMENSIONAL,
    MODEL
} mode;

struct Bar {
    float x;
    float height;

    Bar(float x, float height) : x(x), height(height) {

    }
};

/*
const float bar_vertices[] = {
        1.0f,  1.0f, 0.0f,  // top right
        1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f,   // top left

        1.0f,  1.0f, -0.2f,  // top right
        1.0f, -1.0f, -0.2f,  // bottom right
        -1.0f, -1.0f, -0.2f,  // bottom left
        -1.0f,  1.0f, -0.2f   // top left
};
 */

float bar_vertices[] = {
        // vertex position                     // normal vector
        1.0f,  1.0f, 0.0f,      0.0f, 0.0f, 1.0f,  // top right
        1.0f, -1.0f, 0.0f,      0.0f, 0.0f, 1.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,  // bottom left
        -1.0f,  1.0f, 0.0f,     0.0f, 0.0f, 1.0f,  // top left

        1.0f,  1.0f, -0.2f,     0.0f, 0.0f, -1.0f, // top right
        1.0f, -1.0f, -0.2f,     0.0f, 0.0f, -1.0f, // bottom right
        -1.0f, -1.0f, -0.2f,    0.0f, 0.0f, -1.0f, // bottom left
        -1.0f,  1.0f, -0.2f,    0.0f, 0.0f, -1.0f  // top left
};

const unsigned int flat_bar_indices[] = {  // note that we start from 0!
        0, 1, 2,
        2, 3, 0,
};

const unsigned int volume_bar_indices[] = {  // note that we start from 0!
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Back face
        4, 5, 6,
        6, 7, 4,
        // Left face
        3, 2, 6,
        6, 7, 3,
        // Right face
        0, 1, 5,
        5, 4, 0,
        // Top face
        4, 0, 3,
        3, 7, 4,
        // Bottom face
        1, 5, 6,
        6, 2, 1
};

int main() {
    // Step 1: Initialize normal vectors
    std::vector<glm::vec3> normals(8, glm::vec3(0.0f, 0.0f, 0.0f));

    // Step 2: Calculate face normals
    for (int i = 0; i < 36; i += 3) {
        unsigned int index1 = volume_bar_indices[i];
        unsigned int index2 = volume_bar_indices[i + 1];
        unsigned int index3 = volume_bar_indices[i + 2];

        glm::vec3 v1(bar_vertices[index1 * 6], bar_vertices[index1 * 6 + 1], bar_vertices[index1 * 6 + 2]);
        glm::vec3 v2(bar_vertices[index2 * 6], bar_vertices[index2 * 6 + 1], bar_vertices[index2 * 6 + 2]);
        glm::vec3 v3(bar_vertices[index3 * 6], bar_vertices[index3 * 6 + 1], bar_vertices[index3 * 6 + 2]);

        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        glm::vec3 face_normal = glm::cross(edge1, edge2);

        normals[index1] += face_normal;
        normals[index2] += face_normal;
        normals[index3] += face_normal;
    }

    // Step 3: Normalize normal vectors
    for (int i = 0; i < 8; i++) {
        normals[i] = glm::normalize(normals[i]);
        std::cout << normals[i].x << ' ' << normals[i].y << ' ' << normals[i].z << std::endl;
        bar_vertices[i*6 + 3 + 0] = normals[i].x;
        bar_vertices[i*6 + 3 + 1] = normals[i].y;
        bar_vertices[i*6 + 3 + 2] = normals[i].z;
    }

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

    // Creating OpenGL window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Audio Visualizer");
    glEnable(GL_TEXTURE_2D);
    window.setActive(true);

    // Initialize GLEW
    glewInit();

    // Creating the Shader object
    Shader barShader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");
    Shader lightShader("../shaders/vertex_shader.vert", "../shaders/light_fragment_shader.frag");

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    const float frequencySpectrumToBinsScaleFactor = FFTStream::CONSIDERATION_LENGTH / NUM_BARS;

    // Camera stuff
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    // Draw Wireframes
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // VAO stuff for bars
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bar_vertices) , bar_vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // set vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // set vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(flat_bar_indices), flat_bar_indices, GL_DYNAMIC_DRAW);

    // VAO stuff for light
    glBindVertexArray(lightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Default Mode is 2D
    mode = Mode::TWO_DIMENSIONAL;

    // Model View Projection matrices
    glm::mat4 view_matrix = glm::mat4(1.0f);
    view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -3.0f));
    view_matrix = glm::rotate(view_matrix, 45.0f, glm::vec3(1.0f, 0.0f, 0.0f)); // rotate the camera around the x-axis to look down at the origin
    //view_matrix = glm::rotate(view_matrix, 22.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // rotate the camera around the y-axis to position it to the right
    //view_matrix = glm::rotate(view_matrix, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f)); // rotate the camera around the z-axis to position it up

    //glm::mat4 projection_matrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 100.0f);

    glm::vec3 lightPos(0.0f, 0.5f, 0.0f);
    glm::mat4 light_model_matrix = glm::mat4(1.0f);
    light_model_matrix = glm::translate(light_model_matrix, lightPos);
    light_model_matrix = glm::scale(light_model_matrix, glm::vec3(0.1f));

    sf::Clock clock;
    sf::Time time;

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    // Setting up bars
    float bar_width = (1.0f / NUM_BARS);
    std::vector<Bar> bars;
    bars.reserve(NUM_BARS);
    for (int i = 0 ; i < NUM_BARS ; i++)
        bars.push_back(Bar(-1.0 + ((i + 1) * (bar_width) * 2 - (0.5 * bar_width)), 0.0f));

    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    unsigned long next_game_tick = std::chrono::system_clock::now().time_since_epoch().count();
    unsigned long sleep_time = 0;
    bool running = true;
    while (running) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                // end the program
                running = false;
            } else if (event.type == sf::Event::Resized) {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
                mode = Mode::TWO_DIMENSIONAL;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(flat_bar_indices), flat_bar_indices, GL_DYNAMIC_DRAW);
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
                mode = Mode::TWO_DIMENSIONAL_SPINNING;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(flat_bar_indices), flat_bar_indices, GL_DYNAMIC_DRAW);
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
                mode = Mode::THREE_DIMENSIONAL;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(volume_bar_indices), volume_bar_indices, GL_DYNAMIC_DRAW);
            }
        }

        // Imgui stuff
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::ShowDemoWindow();

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

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

        // Drawing bars
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        barShader.use();

        float angle_of_rotation = clock.getElapsedTime().asSeconds() * glm::radians(-50.0f);

        for (Bar bar : bars) {
            /*
            glm::mat4 view = glm::lookAt(cameraPos,
                                         cameraTarget,
                                         cameraUp);
             */
            glm::mat4 model_matrix = glm::mat4(1.0f);
            if (mode == Mode::TWO_DIMENSIONAL_SPINNING) {
                model_matrix = glm::rotate(model_matrix, angle_of_rotation, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            model_matrix = glm::translate(model_matrix, glm::vec3(bar.x, 0.0, 0.0));
            model_matrix = glm::scale(model_matrix, glm::vec3(bar_width, bar.height, 1.0));

            barShader.setMat4("model", model_matrix);
            barShader.setMat4("view", view_matrix);
            barShader.setMat4("projection", projection_matrix);

            barShader.setVec3f("objectColor", 1.0f, 0.5f, 0.31f);
            barShader.setVec3f("lightColor", 1.0f, 1.0f, 1.0f);
            barShader.setVec3f("lightPos", lightPos.x, lightPos.y, lightPos.z);

            // draw the bar
            glBindVertexArray(VAO);
            if (Mode::THREE_DIMENSIONAL)
                glDrawElements(GL_TRIANGLES, sizeof(volume_bar_indices), GL_UNSIGNED_INT, 0);
            else
                glDrawElements(GL_TRIANGLES, sizeof(flat_bar_indices), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // Draw the light source
        /*
        lightShader.use();

        lightShader.setMat4("model", light_model_matrix);
        lightShader.setMat4("view", view_matrix);
        lightShader.setMat4("projection", projection_matrix);

        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, sizeof(volume_bar_indices), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
         */

        ImGui::SFML::Render(window);

        // end the current frame (internally swaps the front and back buffers)
        window.display();

        // FPS stuff
        // todo: window.setFramerateLimit(60) ???? why not
        next_game_tick += SKIP_TICKS;
        sleep_time = next_game_tick - std::chrono::system_clock::now().time_since_epoch().count();
        if (sleep_time >= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
    }

    ImGui::SFML::Shutdown();

    // todo cleanup stuff?
    /*
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
     */

    return 0;
}