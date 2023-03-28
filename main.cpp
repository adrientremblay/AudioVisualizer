#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <mutex>
#include "../include/FFTStream.h"
#include <SFML/OpenGL.hpp>

constexpr unsigned int WINDOW_WIDTH = 1024;
constexpr unsigned int WINDOW_HEIGHT = 700;
constexpr unsigned int BAR_HEIGHT_SCALING = 2;
constexpr unsigned int BARS = 25;

std::mutex mtx;

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

    // Creating OpenGL window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Audio Visualizer");
    glEnable(GL_TEXTURE_2D);

    // activate the window
    window.setActive(true);

    bool running = true;
    while (running)
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw...

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    /*
    std::vector<sf::RectangleShape> bars;
    bars.reserve(BARS);
    for (int i = 0 ; i < BARS ; i++) {
        sf::RectangleShape rect;
        rect.setFillColor(sf::Color::Green);
        bars.push_back(rect);
    }

    std::vector<float> bar_heights;
    bar_heights.reserve(BARS);

    const float frequencySpectrumToBinsScaleFactor = FFTStream::CONSIDERATION_LENGTH / BARS;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        window.clear();

        for (int i = 0 ; i < BARS ; i++) {
            bar_heights[i] = 0;
        }

        for (int i = 0 ; i < FFTStream::CONSIDERATION_LENGTH ; i++) {
            int bar_index = floor(i / frequencySpectrumToBinsScaleFactor);

            float frequency_mag = abs(normalizedFrequencySpectrum[i]) * BAR_HEIGHT_SCALING;

            bar_heights[bar_index] += frequency_mag;
        }

        for (int i = 0 ; i < BARS ; i++) {
            sf::RectangleShape rect = bars[i];

            rect.setSize(sf::Vector2f(WINDOW_WIDTH / BARS, bar_heights[i]));
            rect.setPosition(sf::Vector2f(i * (WINDOW_WIDTH / BARS), WINDOW_HEIGHT - bar_heights[i]));

            window.draw(rect);
        }

        window.display();
    }
     */
    return 0;
}