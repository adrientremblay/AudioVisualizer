#include <alsa/asoundlib.h>
#include <iostream>
#include "libs/Gist/src/Gist.h"

int main(int argc,char *argv[]) {
    snd_pcm_t *handle;
    /* Open PCM device for recording (capture). */
    int rc = snd_pcm_open(&handle, "hw:0",SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        // keeping this check because this error is actually likely to occur
        fprintf(stderr, "unable to open pcm device: %s\n",snd_strerror(rc));
        exit(1);
    }

    // Setting up ALSA params
    snd_pcm_hw_params_t *params;
    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */

    /* Non Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params,SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed float format */
    snd_pcm_hw_params_set_format(handle, params,SND_PCM_FORMAT_FLOAT);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, 2);

    int dir;
    /* 44100 bits/second sampling rate (CD quality) */
    unsigned int sample_rate = 44100;
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);

    /* Set period frames to 48 */
    snd_pcm_uframes_t frames_single_channel = 48;
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames_single_channel, &dir);

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0)
    {
        fprintf(stderr,"unable to set hw parameters: %s\n",snd_strerror(rc));
        exit(1);
    }


    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params, &frames_single_channel, &dir);
    int frames_per_read = frames_single_channel * 2; /* 2 bytes/sample, 2 channels */
    float *frames_buffer = (float *) malloc(frames_per_read * sizeof(float));

    // Initializing GIST
    Gist<float> gist (2, sample_rate);

    /* We want to loop for 5 seconds */
    snd_pcm_hw_params_get_period_time(params, &sample_rate, &dir);

    snd_pcm_sframes_t frames_actually_read;
    for (int loop = 0 ; loop < (5 * 1000000) / sample_rate ; loop++){
        frames_actually_read = snd_pcm_readi(handle, frames_buffer, frames_single_channel);
        if (frames_actually_read == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (frames_actually_read < 0) {
            fprintf(stderr,"error from read: %s\n", snd_strerror(rc));
        } else if (frames_actually_read != (int)frames_single_channel) {
            fprintf(stderr, "short read, read %d frames_single_channel\n", rc);
        }

        float firstFrame[] = {frames_buffer[0], frames_buffer[1]};
        std::cout << frames_buffer[0] << ' ' << frames_buffer[1] << std::endl;
        gist.processAudioFrame(firstFrame, 2);
        float specCent = gist.spectralCrest();
        std::cout << specCent << std::endl;

        /*
        // Printing buffer
        frames_actually_read = write(1, frames_buffer, frames_per_read);
        if (frames_actually_read != frames_per_read)
            fprintf(stderr, "short write: wrote %d bytes\n", rc);
        */
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(frames_buffer);

    return 0;
}