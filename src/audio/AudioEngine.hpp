// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <unordered_set>

#include "AudioBackend.hpp"
#include "AudioPreset.hpp"
#include "AudioProcessor.hpp"
#include "FFTComputer.hpp"

#define SAMPLE_RATE (44100)

class AudioEngine {
public:
    AudioEngine(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer);

    // Recurse from the root of the tree
    AudioBuffer ProcessBuffer(size_t numFrames);

    void Start(std::atomic<bool>& running);
private:
    void InitChirpAudioProcessorTree();
    void InitSynthAudioProcessorTree();

    std::shared_ptr<AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    AudioBackend m_backend;
    std::shared_ptr<AudioProcessor> m_chirpRoot;
    std::shared_ptr<AudioProcessor> m_synthRoot;
};