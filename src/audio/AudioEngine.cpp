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
#include "AudioEngine.hpp"
#include "AudioBackend.hpp"
#include "Frequency.hpp"
#include "AudioPreset.hpp"
#include "Oscillator.hpp"
#include "Waveform.hpp"
#include "RandomLFO.hpp"
#include "Mixer.hpp"
#include "BaseFilter.hpp"
#include "LowPassFilter.hpp"
#include "HighPassFilter.hpp"
#include "FeedbackDelay.hpp"
#include "Reverb.hpp"

AudioEngine::AudioEngine(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer)
    : m_preset(preset)
    , m_fftComputer(fftComputer)
    , m_backend(this)
{}

// Recurse from the root of the graph
AudioBuffer AudioEngine::ProcessBuffer(size_t numFrames) {
    AudioLayout *selectedLayout = nullptr;
    AppMode selectedMode = m_preset->appMode.load();
    if (selectedMode == AppMode::Chirp) {
        selectedLayout = &m_chirpLayout;
    }else if (selectedMode == AppMode::Synth) {
        selectedLayout = &m_synthLayout;
    }
    assert(selectedLayout != nullptr && "A new app mode was introduced but it doesn't configure any layout.");

    std::shared_ptr<AudioProcessor> rootNode = selectedLayout->GetRootNode();
    if (!rootNode) {
        // Empty processing graph, so provide empty audio
        return AudioBuffer(numFrames);
    }

    selectedLayout->LoadPreset(*m_preset);

    AudioBuffer result(numFrames);
    for (size_t i = 0; i < numFrames; i++) {
        rootNode->ClearVisited();
        rootNode->ClearModulations();
        selectedLayout->ApplyAllModulations();
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
