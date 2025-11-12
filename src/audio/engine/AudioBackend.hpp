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

#include "audio/core/Frequency.hpp"
#include "audio/preset/AudioPreset.hpp"
#include "audio/engine/AudioFrame.hpp"
#include "portaudio.h"

// Forward declaration
class AudioEngine;

class AudioBackend {
public:
    AudioBackend(AudioEngine *engine);

    // Opens a stream for the audio device for playback
    bool Open(PaDeviceIndex index);

    // Closes the stream
    bool Close();
    
    // Starts the stream
    bool Start();

    // Stops the stream
    bool Stop();

private:
    // The instance callback for providing the audio buffer
    int PaCallbackMethod(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);

    // This routine will be called by the PortAudio engine when audio is needed
    static int PaCallback(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);
    
    // The instance callback when playback is done
    void PaStreamFinishedMethod();

    // This routine is called by portaudio when playback is done
    static void PaStreamFinished(void* userData);

    PaStream *m_stream;
    AudioEngine *m_engine;
};