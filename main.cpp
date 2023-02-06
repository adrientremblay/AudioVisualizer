#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include <mutex>
#include "../include/FFTStream.h"

std::vector<std::complex<double>> discreteFourierTransform(std::vector<std::complex<double>> x);
void processSignal(sf::SoundBuffer soundBuffer);

constexpr unsigned int PERIOD = 20;
constexpr unsigned int BINS = PERIOD / 2;

std::mutex mtx;

int main() {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("../audio/raver.wav")) {
        std::cerr << "Could not load RAVER.mp3!!!" << std::endl;
        return -1;
    }

    /*
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
     */

    float normalizedOutputFFT[512];

    FFTStream fftStream;
    fftStream.load(buffer);
    fftStream.setCtx(normalizedOutputFFT);
    fftStream.play();

    sf::RenderWindow window(sf::VideoMode(500, 500), "Audio Visualizer");
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(100, 50));
    rect.setPosition(sf::Vector2f(10, 20));
    rect.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        window.clear();
        window.draw(rect);
        window.display();
    }
    return 0;
}

void processSignal(sf::SoundBuffer soundBuffer) {
    // The samples are in time domain, they represent a sequence of amplitudes -> the position of the speaker cone
    const sf::Int16* samples = soundBuffer.getSamples();
    std::size_t N = soundBuffer.getSampleCount();
    //buffer.saveToFile("my_record.ogg");

    double* fftw_in  = fftw_alloc_real(N);

    for (int i = 0 ; i < N ; i++) {
        fftw_in[i] = static_cast<double>(samples[i]);
    }

    fftw_complex* fftw_out  = fftw_alloc_complex(N);

    // todo: window function

    fftw_plan plan = fftw_plan_dft_r2c_1d(N, fftw_in, fftw_out, FFTW_ESTIMATE);
    fftw_execute(plan);

    // Calculating magnitudes
    std::cout << "==============================" << std::endl;
    for (int i = 0 ; i < N / 2 ; i++) {
        double real = fftw_out[i][0];
        double imag = fftw_out[i][1];
        fftw_out[i][0] = sqrt(real * real + imag * imag);
        fftw_out[i][1] = 0;
        std:: cout << fftw_out[i][0] << std::endl;
    }
    std::cout << "==============================" << std::endl;

    fftw_destroy_plan(plan);
    fftw_free(fftw_out);

}

std::vector<std::complex<double>> discreteFourierTransform(std::vector<std::complex<double>> x) {
    // Determine number of samples
    int N = x.size();

    // Allocate memory for X
    std::vector<std::complex<double>> X;
    X.reserve(N);

    // Allocate memory for internals
    std::complex<double> intSum;

    // Looping
    for (int k = 0 ; k < N ; k++) {
        intSum = std::complex<double>(0, 0);

        for (int n = 0 ; n < N ; n++) {
            double inside = ((2*M_PI) / N) * k * n;
            double realPart = cos(inside);
            double imagPart = sin(inside);
            std::complex<double> w(realPart, -imagPart);
            intSum += x[n] * w;
        }

        X.push_back(intSum);
    }

    return X;
}