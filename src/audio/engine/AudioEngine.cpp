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
#include "engine/AudioEngine.hpp"
#include "engine/AudioBackend.hpp"
#include "core/Frequency.hpp"
#include "preset/AudioPreset.hpp"
#include "generator/Oscillator.hpp"
#include "core/Waveform.hpp"
#include "modulation/RandomLFO.hpp"
#include "effects/Mixer.hpp"
#include "effects/BaseFilter.hpp"
#include "effects/LowPassFilter.hpp"
#include "effects/HighPassFilter.hpp"
#include "effects/FeedbackDelay.hpp"
#include "effects/Reverb.hpp"

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
