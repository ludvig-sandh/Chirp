// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include <iostream>
#include <memory>
#include "audio/engine/AudioEngine.hpp"

namespace Audio::Engine {

AudioEngine::AudioEngine(std::shared_ptr<Audio::Preset::AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer)
    : m_preset(preset)
    , m_fftComputer(fftComputer)
    , m_backend(this)
{}

// Recurse from the root of the graph
AudioBuffer AudioEngine::ProcessBuffer(int numFrames) {
    std::shared_ptr<AudioProcessorNode> rootNode = m_synthLayout.GetRootNode();
    if (!rootNode) {
        // Empty processing graph, so provide empty audio
        return AudioBuffer(numFrames);
    }

    m_synthLayout.LoadPreset(*m_preset);

    AudioBuffer result(numFrames);
    for (auto& frame : result) {
        rootNode->ClearVisited();
        rootNode->ClearModulations();
        m_synthLayout.ApplyAllModulations();
        frame = rootNode->GenerateFrame(*m_preset);
    }

    // Send buffer to FFT thread
    m_fftComputer->ProvideAudioBuffer(result);
    
    return result;
}

void AudioEngine::Start(std::atomic<bool>& running) {
    ScopedPaHandler paInit;
    if (paInit.Result() != paNoError) {
        std::cerr << "An error occurred while using the portaudio stream\n";
        std::cerr << "Error number: %d\n" << paInit.Result();
        std::cerr << "Error message: %s\n" << Pa_GetErrorText(paInit.Result());
    }

    if (m_backend.Open(Pa_GetDefaultOutputDevice())) {
        if (m_backend.Start()) {
            while (running.load()) {
                Pa_Sleep(50); // Sleep 50ms
            }
            m_backend.Stop();
        }
        
        m_backend.Close();
    }

    // Signal the FFT thread that no more audio is coming.
    m_fftComputer->FinishedProducing();
}

} // namespace Audio::Engine
