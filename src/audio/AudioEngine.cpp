/** @file paex_sine.c
    @ingroup examples_src
    @brief Play a sine wave for several seconds.
    @author Ross Bencina <rossb@audiomulch.com>
    @author Phil Burk <philburk@softsynth.com>
*/
/*
 * $Id: paex_sine.c 1752 2011-09-08 03:21:55Z philburk $
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com/
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <thread>

#include "portaudio.h"
#include "AudioPreset.h"

#define NUM_SECONDS   (10)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)

class Frequency {
public:
    Frequency(float hertz) : hertz(hertz), pitch(0.0) {}

    void setPitch(float pitch) {
        // The maximum pitch we should be able to hear is half the sample rate (Nyquist theorem)
        // this->pitch = std::min(pitch, static_cast<float>(SAMPLE_RATE / 2.0));
        this->pitch = pitch;
    }

    float getAbsolute() {
        // The maximum pitch we should support is half the sample rate (Nyquist theorem)
        // Otherwise we'd get foldback aliasing.
        float actual = hertz * std::pow(2.0, pitch / notesPerOctave);
        return std::min(actual, static_cast<float>(SAMPLE_RATE / 2.0));
    }

private:
    float hertz;
    float pitch;
    static inline float notesPerOctave = 12.0;
};

class RandomSignalGenerator {
public:
    RandomSignalGenerator(Frequency freq) {
        numSamplesPerPeriod = SAMPLE_RATE / freq.getAbsolute();
        generateRandValueHelper();
        lastRandValue = generateRandValueHelper();
        nextRandValue = generateRandValueHelper();
    }

    float getNextSample() {
        float t = static_cast<float>(sampleCount) / numSamplesPerPeriod;
        float toReturn = cubicInterpolate(lastRandValue, nextRandValue, t);

        sampleCount++;
        if (sampleCount == numSamplesPerPeriod) {
            sampleCount = 0;
            lastRandValue = nextRandValue;
            nextRandValue = generateRandValueHelper();
        }

        return toReturn;
    }
private:
    float generateRandValueHelper() {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    // Simple cubic Hermite interpolation between y0 and y1
    // t = fraction [0,1]
    // Optional tangents m0 and m1; default to 0 for simple smoothness
    float cubicInterpolate(float y0, float y1, float t, float m0 = 0.0, float m1 = 0.0) {
        float t2 = t * t;
        float t3 = t2 * t;

        float h00 = 2*t3 - 3*t2 + 1;
        float h10 = t3 - 2*t2 + t;
        float h01 = -2*t3 + 3*t2;
        float h11 = t3 - t2;

        return h00*y0 + h10*m0 + h01*y1 + h11*m1;
    }

    float lastRandValue;
    float nextRandValue;
    int sampleCount = 0;
    int numSamplesPerPeriod;
};

class Envelope {
public:
    Envelope() : attack(0.0), hold(0.0), dec(0.0), sus(1.0) {} // "invisible" envelope, just lets the signal pass through
    Envelope(float attack, float hold, float dec, float sus) : attack(attack), hold(hold), dec(dec), sus(sus) {
        assert(sus >= 0.0 && sus <= 1.0 && "");
    }

    float getAmplitude(float timeSinceStart) {
        if (timeSinceStart < attack) {
            return timeSinceStart / attack;
        }
        if (timeSinceStart < attack + hold) {
            return 1.0;
        }
        if (timeSinceStart < attack + hold + dec) {
            float timeSinceDecStart = timeSinceStart - attack - hold;
            return 1.0 - (1.0 - sus) * timeSinceDecStart / dec;
        }
        return sus;
    }

private:
    float attack;
    float hold;
    float dec;
    float sus;
};

class Waveform {
public:
    Waveform(Frequency freq) : frequency(freq), envelope() {}
    Waveform(Frequency freq, Envelope env) : frequency(freq), envelope(env) {}

    float getNextSample() {
        float dt = 1.0 / SAMPLE_RATE;
        timeSinceStart += dt;
        float dOffset = dt * frequency.getAbsolute();
        currentOffset += dOffset;

        // Loop back to always be in range [0, 1]
        currentOffset -= static_cast<int>(currentOffset);
        
        float value = volume * getNextSampleImpl(currentOffset) * envelope.getAmplitude(timeSinceStart);
        return value;
    }

    virtual float getNextSampleImpl(float currentOffset) = 0;

    float volume = 0.02;
    Frequency frequency;
private:
    float currentOffset = 0.0;
    float timeSinceStart = 0.0;
    Envelope envelope;
};

class Sine : public Waveform {
public:
    Sine(Frequency freq) : Waveform(freq) {}
    Sine(Frequency freq, Envelope env) : Waveform(freq, env) {}

    float getNextSampleImpl(float currentOffset) override {
        return std::sin(currentOffset * 2.0 * M_PI);
    }
};

class Square : public Waveform {
public:
    Square(Frequency freq) : Waveform(freq) {}
    Square(Frequency freq, Envelope env) : Waveform(freq, env) {}
    
    float getNextSampleImpl(float currentOffset) override {
        return currentOffset >= 0.5 ? 1.0 : 0.0;
    }
};

class Saw : public Waveform {
public:
    Saw(Frequency freq) : Waveform(freq) {}
    Saw(Frequency freq, Envelope env) : Waveform(freq, env) {}
    
    float getNextSampleImpl(float currentOffset) override {
        return currentOffset;
    }
};

class Mixer
{
public:
    Mixer(std::shared_ptr<AudioPreset> preset)
        : stream(0)
        , randomGen(Frequency(16))
        , sine(Frequency(2220.0))
        , lfo(Frequency(62.0))
        , m_preset(preset)
    {
        sprintf( message, "No Message" );
    }

    bool open(PaDeviceIndex index)
    {
        PaStreamParameters outputParameters;

        outputParameters.device = index;
        if (outputParameters.device == paNoDevice) {
            return false;
        }

        const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
        if (pInfo != 0)
        {
            printf("Output device name: '%s'\r", pInfo->name);
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
            &Mixer::paCallback,
            this            /* Using 'this' for userData so we can cast to Sine* in paCallback method */
            );

        if (err != paNoError)
        {
            /* Failed to open stream to device !!! */
            return false;
        }

        err = Pa_SetStreamFinishedCallback( stream, &Mixer::paStreamFinished );

        if (err != paNoError)
        {
            Pa_CloseStream( stream );
            stream = 0;

            return false;
        }

        return true;
    }

    bool close()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_CloseStream( stream );
        stream = 0;

        return (err == paNoError);
    }


    bool start()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_StartStream( stream );

        return (err == paNoError);
    }

    bool stop()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_StopStream( stream );

        return (err == paNoError);
    }

private:
    /* The instance callback, where we have access to every method/variable in object of class Sine */
    int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags)
    {
        float *out = (float*)outputBuffer;
        unsigned long i;

        (void) timeInfo; /* Prevent unused variable warnings. */
        (void) statusFlags;
        (void) inputBuffer;

        float volume = m_preset->volume.load();

        for( i=0; i<framesPerBuffer; i++ )
        {
            float lfoValue = lfo.getNextSample() * 50 - 0.5; // Shift to [-0.5, 0.5]
            float pitch = randomGen.getNextSample() * 64.0 + lfoValue;
            sine.frequency.setPitch(pitch);
            float value = sine.getNextSample() * volume;
            *out++ = value;  /* left */
            *out++ = value;  /* right */
        }

        return paContinue;
    }

    /* This routine will be called by the PortAudio engine when audio is needed.
    ** It may called at interrupt level on some machines so don't do anything
    ** that could mess up the system like calling malloc() or free().
    */
    static int paCallback( const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData )
    {
        /* Here we cast userData to Sine* type so we can call the instance method paCallbackMethod, we can do that since
           we called Pa_OpenStream with 'this' for userData */
        return ((Mixer*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
            framesPerBuffer,
            timeInfo,
            statusFlags);
    }


    void paStreamFinishedMethod()
    {
        printf( "Stream Completed: %s\n", message );
    }

    /*
     * This routine is called by portaudio when playback is done.
     */
    static void paStreamFinished(void* userData)
    {
        return ((Mixer*)userData)->paStreamFinishedMethod();
    }

    PaStream *stream;
    RandomSignalGenerator randomGen;
    Sine sine;
    Saw lfo;
    std::shared_ptr<AudioPreset> m_preset;
    char message[20];
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

/*******************************************************************/
class AudioEngine {
public:
    AudioEngine(std::shared_ptr<AudioPreset> preset)
        : m_preset(preset)
        , m_mixer(preset)
    {}

    void Start(std::atomic<bool>& running) {
        printf("PortAudio Test: output wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);

        ScopedPaHandler paInit;
        if (paInit.result() != paNoError) {
            std::cerr << "An error occurred while using the portaudio stream\n";
            std::cerr << "Error number: %d\n" << paInit.result();
            std::cerr << "Error message: %s\n" << Pa_GetErrorText( paInit.result() );
        }

        if (m_mixer.open(Pa_GetDefaultOutputDevice())) {
            if (m_mixer.start()) {
                while (running.load()) {
                    Pa_Sleep(50); // Sleep 50ms
                }
                m_mixer.stop();
            }

            m_mixer.close();
            printf("Audio stopped.\n");
        }
    }
private:
    std::shared_ptr<AudioPreset> m_preset;
    Mixer m_mixer;
};