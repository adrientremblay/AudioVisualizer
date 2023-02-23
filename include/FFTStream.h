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

    static const int FREQUENCY_SPECTRUM_LENGTH = PERIOD / 2;

    // we only consider the first half of the frequency spectrum to avoid artifacts explained by the
    // Shannon-Nyquist sampling theorem. See https://youtu.be/yYEMxqreA10?t=785
    static const int CONSIDERATION_LENGTH = FREQUENCY_SPECTRUM_LENGTH / 2;
private:
    std::vector<sf::Int16> m_samples;
    std::size_t m_currentSample;
    fftw_complex input[FREQUENCY_SPECTRUM_LENGTH];
    fftw_complex output[FREQUENCY_SPECTRUM_LENGTH];
    float last_output[FREQUENCY_SPECTRUM_LENGTH];
    float *normalizedFrequencySpectrum;
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
