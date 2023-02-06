#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <complex>

int main() {


    /*
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    if (!sf::SoundBufferRecorder::isAvailable()) {
        std::cerr << "Yo the sound buffer recorder isn't available... fix it." << std::endl;
        return -1;
    }

    std::vector<std::string> devices = sf::SoundRecorder::getAvailableDevices();
    std::string defaultDevice = sf::SoundRecorder::getDefaultDevice();

    sf::SoundBufferRecorder recorder;
    recorder.setDevice(devices[1]);
    recorder.start();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    recorder.stop();
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    //buffer.saveToFile("my_record.ogg");

    const sf::Int16* samples = buffer.getSamples();
    std::size_t count = buffer.getSampleCount();

    sf::SoundBuffer Buffer;
    if (!Buffer.loadFromSamples(samples, count, 1, buffer.getSampleRate())) {
        std::cerr << "Loading failed!" << std::endl;
        return 1;
    }

    sf::Sound Sound;
    Sound.setBuffer(Buffer);
    Sound.setLoop(true);
    Sound.play();
    while (1) {
        sf::sleep(sf::milliseconds(100));
    }

    return 0;
    */
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