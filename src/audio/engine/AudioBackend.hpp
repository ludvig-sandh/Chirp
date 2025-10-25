// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <stdio.h>
#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <numbers>

#include "core/Frequency.hpp"
#include "preset/AudioPreset.hpp"
#include "engine/AudioFrame.hpp"
#include "portaudio.h"

// Forward declaration
class AudioEngine;

struct AudioBuffer {
    std::vector<AudioFrame> outputBuffer;
    size_t numFrames;

    AudioBuffer(size_t numFrames) : outputBuffer(numFrames), numFrames(numFrames) {}

    AudioBuffer(float *initBuffer, size_t numFrames)
        : outputBuffer(numFrames)
        , numFrames(numFrames)
    {
        float *out = initBuffer;
        for (size_t i = 0; i < numFrames; i++) {
            outputBuffer[i].left = *out++;
            outputBuffer[i].right = *out++;
        }
    }
};

class ScopedPaHandler
{
public:
    ScopedPaHandler()
        : _result(Pa_Initialize())
    {
    }
    ~ScopedPaHandler()
    {
        if (_result == paNoError)
        {
            Pa_Terminate();
        }
    }

    PaError result() const { return _result; }

private:
    PaError _result;
};

class AudioBackend {
public:
    AudioBackend(AudioEngine *engine);

    bool open(PaDeviceIndex index);
    bool close();
    bool start();
    bool stop();

private:
    /* The instance callback, where we have access to every method/variable in object of class Sine */
    int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);

    /* This routine will be called by the PortAudio engine when audio is needed.
    ** It may called at interrupt level on some machines so don't do anything
    ** that could mess up the system like calling malloc() or free().
    */
    static int paCallback( const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData );


    void paStreamFinishedMethod();

    /*
     * This routine is called by portaudio when playback is done.
     */
    static void paStreamFinished(void* userData);

    PaStream *stream;
    // RandomSignalGenerator randomGen;
    // Sine sine;
    // Saw lfo;
    char message[200];
    AudioEngine *m_engine;
};