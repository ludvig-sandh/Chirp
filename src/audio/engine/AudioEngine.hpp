// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <unordered_set>

#include "audio/engine/AudioBackend.hpp"
#include "audio/preset/AudioPreset.hpp"
#include "audio/engine/AudioBuffer.hpp"
#include "audio/engine/AudioProcessorNode.hpp"
#include "fft/FFTComputer.hpp"
#include "audio/layout/AudioLayout.hpp"
#include "audio/layout/SynthLayout.hpp"

#define SAMPLE_RATE (44100)

class AudioEngine {
public:
    AudioEngine(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer);

    // Recurse from the root of the tree
    AudioBuffer ProcessBuffer(size_t numFrames);

    void Start(std::atomic<bool>& running);
private:
    std::shared_ptr<AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    AudioBackend m_backend;
    SynthLayout m_synthLayout;
};