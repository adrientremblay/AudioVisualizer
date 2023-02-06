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
private:
    static const int samplesToStream = 2048;
    std::vector<sf::Int16> m_samples;
    std::size_t m_currentSample;
    fftw_complex signal[samplesToStream / 2];
    fftw_complex output[samplesToStream / 2];
    float last_output[512];
    float *normalizedOutputFFT;
    fftw_plan plan;
    float duration = 0;

    virtual bool onGetData(Chunk &data);
    virtual void onSeek(sf::Time timeOffset);

public:
    FFTStream();
    ~FFTStream();
    void setCtx(float *);
    float getDuration();
    void load(const sf::SoundBuffer &buffer);
};

#endif //AUDIOVISUALIZER_FFTSTREAM_H
