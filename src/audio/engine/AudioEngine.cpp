// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include <stdio.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <thread>
#include <unordered_set>

#include "portaudio.h"
#include "audio/engine/AudioEngine.hpp"
#include "audio/core/Frequency.hpp"
#include "audio/preset/AudioPreset.hpp"
#include "audio/generator/Oscillator.hpp"
#include "audio/core/Waveform.hpp"
#include "audio/modulation/RandomLFO.hpp"
#include "audio/effects/Mixer.hpp"
#include "audio/effects/BaseFilter.hpp"
#include "audio/effects/LowPassFilter.hpp"
#include "audio/effects/HighPassFilter.hpp"
#include "audio/effects/FeedbackDelay.hpp"
#include "audio/effects/Reverb.hpp"

AudioEngine::AudioEngine(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer)
    : m_preset(preset)
    , m_fftComputer(fftComputer)
    , m_backend(this)
{}

// Recurse from the root of the graph
AudioBuffer AudioEngine::ProcessBuffer(size_t numFrames) {
    std::shared_ptr<AudioProcessor> rootNode = m_synthLayout.GetRootNode();
    if (!rootNode) {
        // Empty processing graph, so provide empty audio
        return AudioBuffer(numFrames);
    }

    m_synthLayout.LoadPreset(*m_preset);

    AudioBuffer result(numFrames);
    for (size_t i = 0; i < numFrames; i++) {
        rootNode->ClearVisited();
        rootNode->ClearModulations();
        m_synthLayout.ApplyAllModulations();
        result.outputBuffer[i] = rootNode->GenerateFrame(*m_preset);
    }

    // Send buffer to FFT thread
    m_fftComputer->ProvideAudioBuffer(result);
    
    return result;
}

void AudioEngine::Start(std::atomic<bool>& running) {
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
    m_fftComputer->FinishedProducing();
}
