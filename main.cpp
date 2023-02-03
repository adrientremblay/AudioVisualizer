#include <alsa/asoundlib.h>
#include <iostream>

struct audio_data_t {
    double *cava_in;

    int input_buffer_size;
    int cava_buffer_size;

    int format;
    unsigned int rate;
    unsigned int channels;
    char *source;  // alsa device, fifo path or pulse source
    int im;        // input mode alsa, fifo, pulse, portaudio, shmem or sndio
    int terminate; // shared variable used to terminate audio thread
    char error_message[1024];
    int samples_counter;
    int IEEE_FLOAT;
    pthread_mutex_t lock;
};

int main() {
    int error;
    struct audio_data_t* audio_data;
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    snd_pcm_uframes_t buffer_size;
    snd_pcm_uframes_t period_size;
    snd_pcm_uframes_t frames = audio_data->input_buffer_size / 2;

    // todo: what is hw:0?
    error = snd_pcm_open(&capture_handle, "hw:0", SND_PCM_STREAM_CAPTURE, 0);

    if (error) {
        std::cerr << "Failure opening audio interface!" << std::endl;
        return 0;
    }

    std::cout << "Audio interface opened!" << std::endl;

    return 0;
}