//
// Created by adrien on 06/02/23.
//

#ifndef AUDIOVISUALIZER_FFTSTREAM_H
#define AUDIOVISUALIZER_FFTSTREAM_H

#include <SFML/Audio.hpp>
#include <fftw3.h>
#include <math.h>
#include "Mutex.h"

#define REAL 0
#define IMAG 1

class FFTStream : public sf::SoundStream {
public:
    // number of samples to stream every time the function is called;
    static const int PERIOD = 2048;
    static const int SIGNAL_LENGTH = PERIOD / 2;
    static const int BINS = SIGNAL_LENGTH / 2;
private:
    std::vector<sf::Int16> m_samples;
    std::size_t m_currentSample;
    fftw_complex signal[PERIOD / 2];
    fftw_complex output[PERIOD / 2];
    float last_output[BINS];
    float *normalizedOutputFFT;
    fftw_plan plan;
    float duration = 0;

    virtual bool onGetData(Chunk &data);
    virtual void onSeek(sf::Time timeOffset);

    void fourierTransform();

public:
    FFTStream();
    ~FFTStream();
    void setCtx(float *);
    float getDuration();
    void load(const sf::SoundBuffer &buffer);
};

#endif //AUDIOVISUALIZER_FFTSTREAM_H
