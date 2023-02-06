#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <complex>
#include <fftw3.h>

std::vector<std::complex<double>> discreteFourierTransform(std::vector<std::complex<double>> x);

int main() {
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

    // The samples are in time domain, they represent a sequence of amplitudes -> the position of the speaker cone
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    const sf::Int16* samples = buffer.getSamples();
    std::size_t N = buffer.getSampleCount();
    //buffer.saveToFile("my_record.ogg");
    std::cout << N << std::endl;

    double* fftw_in  = fftw_alloc_real(N);

    for (int i = 0 ; i < N ; i++) {
        fftw_in[i] = static_cast<double>(samples[i]);
    }

    fftw_complex* fftw_out  = fftw_alloc_complex(N);

    // todo: window function

    // todo: this casting might be bad
    fftw_plan plan = fftw_plan_dft_r2c_1d(N, fftw_in, fftw_out, FFTW_ESTIMATE);
    fftw_execute(plan);

    const int BIN_SIZE = 5000;
    std::vector<double> bins;

    // Calculating magnitudes
    for (int i = 0 ; i < N / 2 ; i++) {
        double real = fftw_out[i][0];
        double imag = fftw_out[i][1];
        fftw_out[i][0] = sqrt(real * real + imag * imag);
        fftw_out[i][1] = 0;
        std:: cout << fftw_out[i][0] << ' ';
    }

    fftw_destroy_plan(plan);
    fftw_free(fftw_out);


    return 0;
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