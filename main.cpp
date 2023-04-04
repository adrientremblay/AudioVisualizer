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
#include "include/Shader.h"
#include "include/Model.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-OpenGL3.hpp>

constexpr unsigned int WINDOW_WIDTH = 1024;
constexpr unsigned int WINDOW_HEIGHT = 700;
constexpr float BAR_HEIGHT_SCALING = 0.005;
constexpr unsigned int DEFAULT_NUM_BARS = 30;
constexpr unsigned int FRAMES_PER_SECOND = 30;
const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;
constexpr float DEFAULT_SPIN_ANGLE = -50.0f;
constexpr float DEFAULT_FLY_ANGLE = 25.0f;
constexpr float DEFAULT_CIRCLE_RADIUS = 1.0f;

std::mutex mtx;

struct Mode {
    bool is3d;

    bool isSpinning;
    float spinAngle;

    bool cameraFly;
    float flyAngle;

    unsigned int numBars;

    bool circleLayout;
    float circleRadius;

    Mode() : is3d(false), isSpinning(false), spinAngle(DEFAULT_SPIN_ANGLE), cameraFly(false), flyAngle(DEFAULT_FLY_ANGLE), numBars(DEFAULT_NUM_BARS), circleLayout(false), circleRadius(DEFAULT_CIRCLE_RADIUS) {

    }
} mode;

struct Bar {
    float x;
    float y;
    float z;
    float height;

    Bar(float x, float y, float z, float height) : x(x), y(y), z(z), height(height) {

    }
};

float flat_bar_vertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
};

float volume_bar_vertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,

        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
};

void genBars(std::vector<Bar>& bars) {
    const float bar_width = (1.0f / mode.numBars);

    bars.clear();
    bars.reserve(mode.numBars);

    if (mode.circleLayout) {
        double angle = 2 * M_PI / mode.numBars;
        for (int i = 0; i < mode.numBars; i++) {
            float x = mode.circleRadius * cos(i * angle);
            float z = mode.circleRadius * sin(i * angle);
            bars.push_back(Bar(x, 0, z, 0.0f));
        }

    } else {
        for (float i = 0.0f ; i < mode.numBars ; i++) {
            float x = -1.0f + ((i + 1) * (bar_width) * 2.0f - (0.5 * bar_width));
            bars.push_back(Bar(x, 0, 0, 0.0f));
        }
    }
}

int main() {
    // Loading song
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("../audio/raver.wav")) {
        std::cerr << "Could not load RAVER.wav!!!" << std::endl;
        return -1;
    }

    // Creating & Starting frequency analysis stream
    float normalizedFrequencySpectrum[FFTStream::CONSIDERATION_LENGTH];
    FFTStream fftStream;
    fftStream.load(buffer);
    fftStream.setCtx(normalizedFrequencySpectrum);
    fftStream.play();
    fftStream.setVolume(0);
    fftStream.setLoop(true);

    // Creating OpenGL window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Audio Visualizer", sf::Style::Default, sf::ContextSettings(24, 8, 0, 3, 3));
    glEnable(GL_TEXTURE_2D);
    tgui::Gui gui{window};
    window.setActive(true);

    // Depth testing stuff
    glEnable(GL_DEPTH_TEST);

    // Initialize GLEW
    glewInit();

    // Creating the Shader object
    Shader barShader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");
    Shader lightShader("../shaders/vertex_shader.vert", "../shaders/light_fragment_shader.frag");
    Shader monkeyShader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    // Camera stuff
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    // Draw Wireframes
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // VAO stuff for bars
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(flat_bar_vertices) , flat_bar_vertices, GL_DYNAMIC_DRAW);
    // set vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // set vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // VAO stuff for light
    glBindVertexArray(lightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Model View Projection matrices

    //glm::mat4 projection_matrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 100.0f);

    glm::vec3 lightPos(0.0f, 0.5f, 0.0f);
    glm::mat4 light_model_matrix = glm::mat4(1.0f);
    light_model_matrix = glm::translate(light_model_matrix, lightPos);
    light_model_matrix = glm::scale(light_model_matrix, glm::vec3(0.1f));

    sf::Clock clock;
    sf::Time time;

    // Setting up bars
    std::vector<Bar> bars;
    genBars(bars);

    // Setting up GUI
    tgui::Theme::setDefault("../themes/TransparentGrey.txt");

    tgui::CheckBox::Ptr dimension_checkbox = tgui::CheckBox::create();
    dimension_checkbox->onCheck([&]() {
        mode.is3d = true;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(volume_bar_vertices) , volume_bar_vertices, GL_DYNAMIC_DRAW);
    });
    dimension_checkbox->onUncheck([&]() {
        mode.is3d = false;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(flat_bar_vertices) , flat_bar_vertices, GL_DYNAMIC_DRAW);
    });
    gui.add(dimension_checkbox);

    tgui::Label::Ptr  checkbox_label = tgui::Label::create("3D Mode");
    checkbox_label->setPosition(20, 0);
    gui.add(checkbox_label);

    tgui::CheckBox::Ptr spin_checkbox = tgui::CheckBox::create();

    spin_checkbox->onCheck([&]() {
        mode.isSpinning = true;
    });
    spin_checkbox->onUncheck([&]() {
        mode.isSpinning = false;
    });
    spin_checkbox->setPosition(0, 20);
    gui.add(spin_checkbox);

    tgui::Label::Ptr spin_label = tgui::Label::create("Spin");
    spin_label->setPosition(20, 20);
    gui.add(spin_label);

    tgui::EditBox::Ptr spin_angle_edit_box = tgui::EditBox::create();
    spin_angle_edit_box->setSize(40, 20);
    spin_angle_edit_box->setPosition(100, 20);
    spin_angle_edit_box->onReturnKeyPress([&] {
        float spin_angle_edit_box_value = spin_angle_edit_box->getText().toFloat();
        if (spin_angle_edit_box_value < -360.0f || spin_angle_edit_box_value > 360.0f)
            return;
        mode.spinAngle = spin_angle_edit_box_value;
    });
    gui.add(spin_angle_edit_box);

    tgui::Label::Ptr spin_angle_label = tgui::Label::create("Spin Angle");
    spin_angle_label->setPosition(150, 20);
    gui.add(spin_angle_label);

    tgui::CheckBox::Ptr camera_fly_checkbox = tgui::CheckBox::create();
    camera_fly_checkbox->onCheck([&]() {
        mode.cameraFly = true;
    });
    camera_fly_checkbox->onUncheck([&]() {
        mode.cameraFly = false;
    });
    camera_fly_checkbox->setPosition(0, 40);
    gui.add(camera_fly_checkbox);

    tgui::Label::Ptr camera_fly_label = tgui::Label::create("Camera Fly");
    camera_fly_label->setPosition(20, 40);
    gui.add(camera_fly_label);

    tgui::EditBox::Ptr fly_angle_edit_box = tgui::EditBox::create();
    fly_angle_edit_box->setSize(40, 20);
    fly_angle_edit_box->setPosition(100, 40);
    fly_angle_edit_box->onReturnKeyPress([&] {
        float fly_angle_edit_box_value = fly_angle_edit_box->getText().toFloat();
        if (fly_angle_edit_box_value < -360.0f || fly_angle_edit_box_value > 360.0f)
            return;
        mode.flyAngle = fly_angle_edit_box_value;
    });
    gui.add(fly_angle_edit_box);

    tgui::Label::Ptr fly_angle_label = tgui::Label::create("Fly Angle");
    fly_angle_label->setPosition(150, 40);
    gui.add(fly_angle_label);

    tgui::EditBox::Ptr num_bars_edit_box = tgui::EditBox::create();
    num_bars_edit_box->setSize(40, 20);
    num_bars_edit_box->setPosition(100, 60);
    num_bars_edit_box->onReturnKeyPress([&] {
        unsigned int num_bars_value = num_bars_edit_box->getText().toUInt();
        if (num_bars_value > FFTStream::CONSIDERATION_LENGTH || num_bars_value == 0)
            return;
        mode.numBars = num_bars_value;
        genBars(bars);
    });
    gui.add(num_bars_edit_box);

    tgui::Label::Ptr num_bars_label = tgui::Label::create("Num Bars");
    num_bars_label->setPosition(150, 60);
    gui.add(num_bars_label);

    tgui::CheckBox::Ptr circle_layout_checkbox = tgui::CheckBox::create();
    circle_layout_checkbox->onCheck([&]() {
        mode.circleLayout = true;
        genBars(bars);
    });
    circle_layout_checkbox->onUncheck([&]() {
        mode.circleLayout = false;
        genBars(bars);
    });
    circle_layout_checkbox->setPosition(0, 80);
    gui.add(circle_layout_checkbox);

    tgui::Label::Ptr circle_layout_label = tgui::Label::create("Circle");
    circle_layout_label->setPosition(20, 80);
    gui.add(circle_layout_label);

    // Model loading
    Model suzanne(std::string("../models/suzanne.obj").c_str());
    glm::mat4 monkey_model_matrix(1.0f);
    monkey_model_matrix = glm::scale(monkey_model_matrix, glm::vec3(0.25f, 0.25f, 0.25f));

    sf::Clock deltaClock;
    unsigned long next_game_tick = std::chrono::system_clock::now().time_since_epoch().count();
    unsigned long sleep_time = 0;
    bool running = true;
    while (running) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            gui.handleEvent(event);

            if (event.type == sf::Event::Closed) {
                // end the program
                running = false;
            } else if (event.type == sf::Event::Resized) {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // Resetting bar height of all bars to 0
        for (int i = 0 ; i < mode.numBars ; i++) {
            bars.at(i).height = 0;
        }

        // Calculating bar heights
        const float frequencySpectrumToBinsScaleFactor = FFTStream::CONSIDERATION_LENGTH / (float)mode.numBars;
        for (int i = 0 ; i < FFTStream::CONSIDERATION_LENGTH ; i++) {
            int bar_index = floor(i / frequencySpectrumToBinsScaleFactor);

            float frequency_mag = abs(normalizedFrequencySpectrum[i]) * BAR_HEIGHT_SCALING;

            bars.at(bar_index).height += frequency_mag;
        }

        // Drawing bars
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        barShader.use();

        float angle_of_rotation = clock.getElapsedTime().asSeconds() * glm::radians(mode.spinAngle);

        glm::mat4 view_matrix = glm::mat4(1.0f);
        view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -3.0f));
        view_matrix = glm::rotate(view_matrix, 45.0f, glm::vec3(1.0f, 0.0f, 0.0f)); // rotate the camera around the x-axis to look down at the origin
        if (mode.cameraFly) {
            float fly_angle_of_rotation = clock.getElapsedTime().asSeconds() * glm::radians(mode.flyAngle);
            view_matrix = glm::rotate(view_matrix, fly_angle_of_rotation, glm::vec3(0.0f, 1.0f, 0.0f)); // rotate the camera around the y-axis to position it to the right
        }
        //view_matrix = glm::rotate(view_matrix, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f)); // rotate the camera around the z-axis to position it up

        const float bar_width = (1.0f / mode.numBars);

        for (Bar bar : bars) {
            /*
            glm::mat4 view = glm::lookAt(cameraPos,
                                         cameraTarget,
                                         cameraUp);
             */
            glm::mat4 model_matrix = glm::mat4(1.0f);
            if (mode.isSpinning) {
                model_matrix = glm::rotate(model_matrix, angle_of_rotation, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            model_matrix = glm::translate(model_matrix, glm::vec3(bar.x, bar.y, bar.z));
            model_matrix = glm::scale(model_matrix, glm::vec3(bar_width, bar.height, bar_width));

            barShader.setMat4("model", model_matrix);
            barShader.setMat4("view", view_matrix);
            barShader.setMat4("projection", projection_matrix);

            barShader.setVec3f("objectColor", 1.0f, 0.5f, 0.31f);
            barShader.setVec3f("lightColor", 1.0f, 1.0f, 1.0f);
            barShader.setVec3f("lightPos", lightPos.x, lightPos.y, lightPos.z);

            // draw the bar
            glBindVertexArray(VAO);
            if (mode.is3d)
                glDrawArrays(GL_TRIANGLES, 0, 36);
            else
                glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        // drawing models
        monkeyShader.setMat4("model", monkey_model_matrix);
        monkeyShader.setMat4("view", view_matrix);
        monkeyShader.setMat4("projection", projection_matrix);

        monkeyShader.setVec3f("objectColor", 0.31f, 1.0f, 0.31f);
        monkeyShader.setVec3f("lightColor", 1.0f, 1.0f, 1.0f);
        monkeyShader.setVec3f("lightPos", lightPos.x, lightPos.y, lightPos.z);
        suzanne.Draw(monkeyShader);

        // Draw the light source
        lightShader.use();

        lightShader.setMat4("model", light_model_matrix);
        lightShader.setMat4("view", view_matrix);
        lightShader.setMat4("projection", projection_matrix);

        glBindVertexArray(lightVAO);
        if (mode.is3d)
            glDrawArrays(GL_TRIANGLES, 0, 6);
        else
            glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // drawing gui
        gui.draw();

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