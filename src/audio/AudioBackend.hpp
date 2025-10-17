#pragma once

#include <stdio.h>
#include <algorithm>
#include <memory>
#include <unordered_set>

#include "Frequency.hpp"
#include "AudioPreset.hpp"
#include "portaudio.h"

// Forward declaration
class AudioEngine;

struct AudioBuffer {
    float *outputBuffer;
    unsigned long framesPerBuffer;
    const PaStreamCallbackTimeInfo *timeInfo;
    const PaStreamCallbackFlags *statusFlags;
};

struct AudioBufferFrame {
    float left;
    float right;

    // Clamp the amplitude to avoid the possibility of going deaf
    void ClipToValidRange() {
        left = std::clamp(left, -1.0f, 1.0f);
        right = std::clamp(right, -1.0f, 1.0f);
    }

    static AudioBufferFrame Blend(const AudioBufferFrame& processed, const AudioBufferFrame& unprocessed, float mix) {
        mix = std::clamp(mix, 0.0f, 1.0f);
        float leftBlended = processed.left * mix + unprocessed.left * (1.0f - mix);
        float rightBlended = processed.right * mix + unprocessed.right * (1.0f - mix);
        return AudioBufferFrame{leftBlended, rightBlended};
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