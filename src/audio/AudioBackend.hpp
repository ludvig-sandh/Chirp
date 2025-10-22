#pragma once

#include <stdio.h>
#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <numbers>

#include "Frequency.hpp"
#include "AudioPreset.hpp"
#include "portaudio.h"

// Forward declaration
class AudioEngine;

struct AudioBufferFrame {
    float left = 0.0f;
    float right = 0.0f;

    // Clamp the amplitude to avoid the possibility of going deaf
    void ClipToValidRange() {
        left = std::clamp(left, -1.0f, 1.0f);
        right = std::clamp(right, -1.0f, 1.0f);
    }

    static AudioBufferFrame Blend(const AudioBufferFrame& processed, const AudioBufferFrame& unprocessed, float mix) {
        mix = std::clamp(mix, 0.0f, 1.0f);

        // Equal-power dry/wet mixing
        float dryGain = std::cos(mix * static_cast<float>(std::numbers::pi / 2.0));
        float wetGain = std::sin(mix * static_cast<float>(std::numbers::pi / 2.0));
        float leftBlended = processed.left * wetGain + unprocessed.left * dryGain;
        float rightBlended = processed.right * wetGain + unprocessed.right * dryGain;
        return AudioBufferFrame{leftBlended, rightBlended};
    }
};

struct AudioBuffer {
    std::vector<AudioBufferFrame> outputBuffer;
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

    void Add(const AudioBuffer& other) {
        assert(numFrames == other.numFrames && "ERROR: Tried to add two audio buffers of different sizes together.");
        for (size_t i = 0; i < numFrames; i++) {
            outputBuffer[i].left += other.outputBuffer[i].left;
            outputBuffer[i].right += other.outputBuffer[i].right;
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