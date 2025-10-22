// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include <stdio.h>
#include <memory>
#include <unordered_set>

#include "AudioBackend.hpp"
#include "AudioEngine.hpp"
#include "Frequency.hpp"
#include "AudioPreset.hpp"
#include "portaudio.h"

AudioBackend::AudioBackend(AudioEngine *engine)
    : stream(0)
    , m_engine(engine)
{
    // sprintf( message, "No Message" );
}

bool AudioBackend::open(PaDeviceIndex index)
{
    PaStreamParameters outputParameters;

    outputParameters.device = index;
    if (outputParameters.device == paNoDevice) {
        return false;
    }

    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
    if (pInfo != 0)
    {
        // printf("Output device name: '%s'\r", pInfo->name);
    }

    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(
        &stream,
        NULL, /* no input */
        &outputParameters,
        SAMPLE_RATE,
        paFramesPerBufferUnspecified,
        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
        &AudioBackend::paCallback,
        this            /* Using 'this' for userData so we can cast to Sine* in paCallback method */
        );

    if (err != paNoError)
    {
        /* Failed to open stream to device !!! */
        return false;
    }

    err = Pa_SetStreamFinishedCallback( stream, &AudioBackend::paStreamFinished );

    if (err != paNoError)
    {
        Pa_CloseStream( stream );
        stream = 0;

        return false;
    }

    return true;
}

bool AudioBackend::close()
{
    if (stream == 0)
        return false;

    PaError err = Pa_CloseStream( stream );
    stream = 0;

    return (err == paNoError);
}


bool AudioBackend::start()
{
    if (stream == 0)
        return false;

    PaError err = Pa_StartStream( stream );

    return (err == paNoError);
}

bool AudioBackend::stop()
{
    if (stream == 0)
        return false;

    PaError err = Pa_StopStream( stream );

    return (err == paNoError);
}

int AudioBackend::paCallbackMethod(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags)
{
    // Process the entire audio graph
    AudioBuffer buffer = m_engine->ProcessBuffer(framesPerBuffer);

    // Copy result back to output buffer
    float *out = (float*)outputBuffer;
    for (const AudioFrame& frame : buffer.outputBuffer) {
        *out++ = frame.left;
        *out++ = frame.right;
    }

    return paContinue;
}

int AudioBackend::paCallback( const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{
    /* Here we cast userData to Sine* type so we can call the instance method paCallbackMethod, we can do that since
        we called Pa_OpenStream with 'this' for userData */
    return ((AudioBackend*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
        framesPerBuffer,
        timeInfo,
        statusFlags);
}

void AudioBackend::paStreamFinishedMethod()
{
    // printf( "Stream Completed: %s\n", message );
}

void AudioBackend::paStreamFinished(void* userData)
{
    return ((AudioBackend*)userData)->paStreamFinishedMethod();
}