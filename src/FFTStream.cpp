//
// Created by adrien on 06/02/23.
//

#include "../include/FFTStream.h"
#include "../include/Mutex.h"

FFTStream::FFTStream() {
    plan = fftw_plan_dft_1d(PERIOD / 2,
                            signal,
                            output,
                            FFTW_FORWARD,
                            FFTW_ESTIMATE);

}

FFTStream::~FFTStream() {
    fftw_destroy_plan(plan);
}

void FFTStream::setCtx(float *normalizedOutput) {
    normalizedOutputFFT = normalizedOutput;
    for (int i = 0; i < BINS; i++) {
        normalizedOutputFFT[i] = 0;
        last_output[i] = 0;
    }

}
void FFTStream::load(const sf::SoundBuffer &buffer) {
    m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());
    m_currentSample = 0;
    initialize(buffer.getChannelCount(), buffer.getSampleRate());
    duration = buffer.getDuration().asSeconds();
}

float FFTStream::getDuration() {
    return duration;
}

bool FFTStream::onGetData(Chunk &data) {
    data.samples = &m_samples[m_currentSample];

    if (m_currentSample + PERIOD <= m_samples.size()) {
        int j = 0;
        for (int i = m_currentSample; i < m_currentSample + PERIOD; i+=2) {
            signal[j][REAL] = 0.5f * (float(m_samples[i]) / 32767.0f + float(m_samples[i + 1]) / 32767.0f);
            signal[j][IMAG] = 0;
            j++;
        }

        double peak = 0;
        for (int i = 0; i < BINS; i++) {
            double amp = sqrt(output[i][REAL] * output[i][REAL] +
                              output[i][IMAG] * output[i][IMAG]);
            peak = std::max(peak, amp);
        }

        fftw_execute(plan);

        std::lock_guard<std::mutex> lock(mtx);
        for (int i = 0; i < BINS; i++) {
            double amp = sqrt(output[i][REAL] * output[i][REAL] + output[i][IMAG] * output[i][IMAG]);

            float avg = (amp + last_output[i]) / 2;

            normalizedOutputFFT[i] = 20 * log10(avg / peak);
            last_output[i] = avg;
        }

        data.sampleCount = PERIOD;
        m_currentSample += PERIOD;

        return true;
    } else {
        data.sampleCount = m_samples.size() - m_currentSample;
        m_currentSample = m_samples.size();

        return false;
    }

    /*
    // number of samples to stream every time the function is called;
    // in a more robust implementation, it should be a fixed
    // amount of time rather than an arbitrary number of samples
    const int PERIOD = 50000;

    // set the pointer to the next audio samples to be played
    data.samples = &m_samples[m_currentSample];

    // have we reached the end of the sound?
    if (m_currentSample + PERIOD <= m_samples.size()) {
        // end not reached: stream the samples and continue
        data.sampleCount = PERIOD;
        m_currentSample += PERIOD;
        return true;
    } else {
        // end of stream reached: stream the remaining samples and stop playback
        data.sampleCount = m_samples.size() - m_currentSample;
        m_currentSample = m_samples.size();
        return false;
    }
    */
}

void FFTStream::onSeek(sf::Time timeOffset) {
    m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
}
