#include <alsa/asoundlib.h>
#include <iostream>
#include <fftw3.h>

int main() {
    int i;
    char* buffer;
    int buffer_frames = 128;
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

    if ((error = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        return -1;
    }
    fprintf(stdout, "hw_params allocated\n");

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

    fprintf(stdout, "hw_params setted\n");

    snd_pcm_hw_params_free(hw_params);

    fprintf(stdout, "hw_params freed\n");

    if ((error = snd_pcm_prepare (capture_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
                 snd_strerror (error));
        exit (1);
    }

    fprintf(stdout, "audio interface prepared\n");

    buffer = static_cast<char *>(malloc(128 * snd_pcm_format_width(format) / 8 * 2));

    fprintf(stdout, "buffer allocated\n");

    for (i = 0; i < 10; ++i) {
        if ((error = snd_pcm_readi (capture_handle, buffer, buffer_frames)) != buffer_frames) {
            fprintf (stderr, "read from audio interface failed (%s)\n",
                     error, snd_strerror (error));
            exit (1);
        }
        fprintf(stdout, "read %d done\n", i);
    }

    std::cout << buffer << std::endl;

    fftw_complex *in, *out;
    fftw_plan p;

    int N = sizeof(buffer);
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(p); /* repeat as needed */

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    free(buffer);

    fprintf(stdout, "buffer freed\n");

    snd_pcm_close (capture_handle);
    fprintf(stdout, "audio interface closed\n");

    return 0;
}