// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include <stdio.h>
#include <memory>
#include <unordered_set>

#include "audio/engine/AudioBackend.hpp"
#include "audio/engine/AudioEngine.hpp"

namespace Audio::Engine {

AudioBackend::AudioBackend(AudioEngine *engine)
    : m_stream(0)
    , m_engine(engine)
{}

bool AudioBackend::Open(PaDeviceIndex index) {
    PaStreamParameters outputParameters;

    outputParameters.device = index;
    if (outputParameters.device == paNoDevice) {
        return false;
    }

    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
    if (pInfo != 0) {
        printf("Output device name: '%s'\r", pInfo->name);
    }

    outputParameters.channelCount = 2; // stereo output
    outputParameters.sampleFormat = paFloat32; // 32 bit floating point output
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(
        &m_stream,
        NULL, // no input
        &outputParameters,
        Audio::Engine::Constants::SAMPLE_RATE,
        paFramesPerBufferUnspecified,
        paClipOff, // we won't output out of range samples so don't bother clipping them
        &AudioBackend::PaCallback,
        this // Using 'this' for userData so we can cast to Sine* in paCallback method
    );

    if (err != paNoError) {
        // Failed to open stream to device
        return false;
    }

    err = Pa_SetStreamFinishedCallback(m_stream, &AudioBackend::PaStreamFinished);

    if (err != paNoError) {
        Pa_CloseStream(m_stream);
        m_stream = 0;
        return false;
    }

    return true;
}

bool AudioBackend::Close() {
    if (m_stream == 0) {
        return false;
    }

    PaError err = Pa_CloseStream(m_stream);
    m_stream = 0;

    return (err == paNoError);
}

bool AudioBackend::Start() {
    if (m_stream == 0)
        return false;

    PaError err = Pa_StartStream(m_stream);

    return (err == paNoError);
}

bool AudioBackend::Stop() {
    if (m_stream == 0)
        return false;

    PaError err = Pa_StopStream(m_stream);

    return (err == paNoError);
}

int AudioBackend::PaCallbackMethod(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags) {
    (void)inputBuffer;
    (void)timeInfo;
    (void)statusFlags;

    // Process the entire audio graph
    AudioBuffer buffer = m_engine->ProcessBuffer(framesPerBuffer);

    // Copy result back to output buffer
    float *out = (float*)outputBuffer;
    for (const auto& frame : buffer) {
        *out++ = frame.left;
        *out++ = frame.right;
    }

    return paContinue;
}

int AudioBackend::PaCallback( const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData ) {
    // Here we cast userData to AudioBackend* type so we can call the instance method PaCallbackMethod, we can do that since
    // we called Pa_OpenStream with 'this' for userData
    return ((AudioBackend*)userData)->PaCallbackMethod(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags);
}

void AudioBackend::PaStreamFinishedMethod() {
    // I currently don't need to do anything here so empty
}

void AudioBackend::PaStreamFinished(void* userData) {
    return ((AudioBackend*)userData)->PaStreamFinishedMethod();
}

} // namespace Audio::Engine
