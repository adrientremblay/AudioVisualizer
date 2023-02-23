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
private:
    std::vector<sf::Int16> m_samples;
    std::size_t m_currentSample;
    fftw_complex signal[SIGNAL_LENGTH];
    fftw_complex output[SIGNAL_LENGTH];
    float last_output[SIGNAL_LENGTH];
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
