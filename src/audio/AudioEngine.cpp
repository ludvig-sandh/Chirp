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
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <thread>
#include <unordered_set>

#include "AudioEngine.hpp"
#include "AudioBackend.hpp"
#include "Frequency.hpp"
#include "AudioPreset.hpp"
#include "portaudio.h"


// class RandomSignalGenerator {
// public:
//     RandomSignalGenerator(Frequency freq) {
//         numSamplesPerPeriod = SAMPLE_RATE / freq.getAbsolute();
//         generateRandValueHelper();
//         lastRandValue = generateRandValueHelper();
//         nextRandValue = generateRandValueHelper();
//     }

//     float getNextSample() {
//         float t = static_cast<float>(sampleCount) / numSamplesPerPeriod;
//         float toReturn = cubicInterpolate(lastRandValue, nextRandValue, t);

//         sampleCount++;
//         if (sampleCount == numSamplesPerPeriod) {
//             sampleCount = 0;
//             lastRandValue = nextRandValue;
//             nextRandValue = generateRandValueHelper();
//         }

//         return toReturn;
//     }
// private:
//     float generateRandValueHelper() {
//         return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//     }

//     // Simple cubic Hermite interpolation between y0 and y1
//     // t = fraction [0,1]
//     // Optional tangents m0 and m1; default to 0 for simple smoothness
//     float cubicInterpolate(float y0, float y1, float t, float m0 = 0.0, float m1 = 0.0) {
//         float t2 = t * t;
//         float t3 = t2 * t;

//         float h00 = 2*t3 - 3*t2 + 1;
//         float h10 = t3 - 2*t2 + t;
//         float h01 = -2*t3 + 3*t2;
//         float h11 = t3 - t2;

//         return h00*y0 + h10*m0 + h01*y1 + h11*m1;
//     }

//     float lastRandValue;
//     float nextRandValue;
//     int sampleCount = 0;
//     int numSamplesPerPeriod;
// };

AudioEngine::AudioEngine(std::shared_ptr<AudioPreset> preset)
    : m_preset(preset)
    , m_backend(this)
{
    InitAudioProcessorTree();
}

// Recurse from the root of the tree
void AudioEngine::ProcessBuffer(const AudioBuffer &buffer) {
    if (m_rootProcessor) {
        m_rootProcessor->Process(buffer, *m_preset.get());
    }
}

void AudioEngine::Start(std::atomic<bool>& running) {
    printf("PortAudio Test: output wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);

    ScopedPaHandler paInit;
    if (paInit.result() != paNoError) {
        std::cerr << "An error occurred while using the portaudio stream\n";
        std::cerr << "Error number: %d\n" << paInit.result();
        std::cerr << "Error message: %s\n" << Pa_GetErrorText( paInit.result() );
    }

    if (m_backend.open(Pa_GetDefaultOutputDevice())) {
        if (m_backend.start()) {
            while (running.load()) {
                Pa_Sleep(50); // Sleep 50ms
            }
            m_backend.stop();
        }

        m_backend.close();
        printf("Audio stopped.\n");
    }
}

void AudioEngine::InitAudioProcessorTree() {
    // TODO: Read configuration from file?
    m_rootProcessor = std::make_shared<Sine>(Frequency(440));

    m_rootProcessor->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        dynamic_cast<Waveform *>(self)->outputVolume = preset.volume.load();
    });
}