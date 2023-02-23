#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <mutex>
#include "../include/FFTStream.h"

constexpr unsigned int WINDOW_WIDTH = 1024;
constexpr unsigned int WINDOW_HEIGHT = 700;
constexpr unsigned int BAR_HEIGHT_SCALING = 2;

std::mutex mtx;

int main() {
    std::cout << std::numeric_limits<float>::max() << std::endl;


    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("../audio/raver.wav")) {
        std::cerr << "Could not load RAVER.mp3!!!" << std::endl;
        return -1;
    }

    float normalizedFrequencySpectrum[FFTStream::CONSIDERATION_LENGTH];

    FFTStream fftStream;
    fftStream.load(buffer);
    fftStream.setCtx(normalizedFrequencySpectrum);
    fftStream.play();

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Audio Visualizer");

    std::vector<sf::RectangleShape> bins;
    bins.reserve(FFTStream::CONSIDERATION_LENGTH);
    for (int i = 0 ; i < FFTStream::CONSIDERATION_LENGTH ; i++) {
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(WINDOW_WIDTH / FFTStream::CONSIDERATION_LENGTH, i));
        rect.setPosition(sf::Vector2f(i * (WINDOW_WIDTH / FFTStream::CONSIDERATION_LENGTH), WINDOW_HEIGHT - i));
        rect.setFillColor(sf::Color::Green);
        bins.push_back(rect);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        window.clear();

        for (int i = 0 ; i < FFTStream::CONSIDERATION_LENGTH ; i++) {
            sf::RectangleShape rect = bins[i];

            // todo: determine if this is necessary

            float bar_height = abs(normalizedFrequencySpectrum[i]) * BAR_HEIGHT_SCALING;

            rect.setSize(sf::Vector2f(WINDOW_WIDTH / FFTStream::CONSIDERATION_LENGTH, bar_height));
            rect.setPosition(sf::Vector2f(i * (WINDOW_WIDTH / FFTStream::CONSIDERATION_LENGTH), WINDOW_HEIGHT - bar_height));

            window.draw(rect);
        }

        window.display();
    }
    return 0;
}