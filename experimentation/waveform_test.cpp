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
#include "portaudio.h"

#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)

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

class Frequency {
public:
    Frequency(float hertz) : hertz(hertz), pitch(1.0) {}

    void setPitch(float pitch) {
        this->pitch = pitch;
    }

    float getAbsolute() {
        return hertz * std::pow(2.0, pitch / notesPerOctave);
    }

private:
    float hertz;
    float pitch;
    static inline float notesPerOctave = 12.0;
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
        if (currentOffset > 1.0) {
            currentOffset -= static_cast<int>(currentOffset);
        }

        float value = volume * getNextSampleImpl(currentOffset) * envelope.getAmplitude(timeSinceStart);
        return value;
    }

    virtual float getNextSampleImpl(float currentOffset) = 0;

    float volume = 0.05;
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
    Mixer() : stream(0)
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

        int secondsSinceStart = static_cast<int>(timeInfo->currentTime);
        int turn = secondsSinceStart % 3;
        if (turn != currentTurn) {
            currentTurn = turn;

            // Play new note
            Frequency freq(440.0);
            Envelope env1(1.0, 0.0, 0.0, 0.0);
            Envelope env2(0.2, 0.0, 0.5, 0.0);
            Envelope env3(0.0, 0.0, 0.2, 0.0);
            switch (currentTurn)
            {
            case 0:
                waveform = std::make_unique<Sine>(freq, env1);
                break;
            case 1:
                waveform = std::make_unique<Square>(freq, env2);
                break;
            case 2:
                waveform = std::make_unique<Saw>(freq, env3);
                break;
            default:
                break;
            }
        }

        float timeIntoThisSecond = timeInfo->currentTime-(long long)timeInfo->currentTime;
        for( i=0; i<framesPerBuffer; i++ )
        {
            timeIntoThisSecond += 1 / SAMPLE_RATE;
            waveform->frequency.setPitch(timeIntoThisSecond * 12.0);
            float value = waveform->getNextSample();
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
    std::unique_ptr<Waveform> waveform;
    int currentTurn = -1;
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
int main(void)
{
    Mixer mixer;

    printf("PortAudio Test: output wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);

    ScopedPaHandler paInit;
    if( paInit.result() != paNoError ) goto error;

    if (mixer.open(Pa_GetDefaultOutputDevice()))
    {
        if (mixer.start())
        {
            printf("Play for %d seconds.\n", NUM_SECONDS );
            Pa_Sleep( NUM_SECONDS * 1000 );

            mixer.stop();
        }

        mixer.close();
    }

    printf("Test finished.\n");
    return paNoError;

error:
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", paInit.result() );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( paInit.result() ) );
    return 1;
}