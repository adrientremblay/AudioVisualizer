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
    int i;
    char* buffer;
    unsigned int rate = 44100;
    int error;
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    struct audio_data_t* audio_data;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

    // todo: what is hw:0?
    error = snd_pcm_open(&capture_handle, "hw:0", SND_PCM_STREAM_CAPTURE, 0);
    if (error) {
        std::cerr << "Failure opening audio interface!" << std::endl;
        return -1;
    }
    std::cout << "Audio interface opened!" << std::endl;

    error = snd_pcm_hw_params_any(capture_handle, hw_params);
    if (error < 0) {
        std::cerr << "Failure initializing hardware parameter structure" << std::endl;
        return -1;
    }
    std::cout << "Hardware parameters initialized!";


    if ((error = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        return -1;
    }

    fprintf(stdout, "hw_params access setted\n");

    if ((error = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
         return -1;
    }

    fprintf(stdout, "hw_params format setted\n");

    if ((error = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
        return -1;
    }

    fprintf(stdout, "hw_params rate setted\n");

    if ((error = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 2)) < 0) {
         return -1;
    }

    fprintf(stdout, "hw_params channels setted\n");

    if ((error = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
        return -1;
    }

    return 0;
}