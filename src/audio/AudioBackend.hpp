#pragma once

#include <stdio.h>
#include <memory>
#include <unordered_set>

#include "Frequency.hpp"
#include "AudioPreset.hpp"
#include "portaudio.h"

// Forward declaration
class AudioEngine;

struct AudioBuffer {
    const float *inputBuffer;
    float *outputBuffer;
    unsigned long framesPerBuffer;
    const PaStreamCallbackTimeInfo *timeInfo;
    const PaStreamCallbackFlags *statusFlags;
};

struct AudioBufferFrame {
    const float * inputFrame;
    float *outputFrame; // Only supposed to write to this
    unsigned long framesPerBuffer;
    unsigned long frameIdx;
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