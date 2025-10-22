// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <functional>
#include <optional>

#include "Frequency.hpp"
#include "AudioBackend.hpp"
#include "LFO.hpp"
#include "Gain.hpp"
#include "Pan.hpp"

// Represents a node in a tree showing how audio is routed throughout the engine.
class AudioProcessor {
public:
    AudioProcessor() = default;
    virtual ~AudioProcessor() = default;

    void AddChild(std::shared_ptr<AudioProcessor> child);
    
    void SetCallbackForReadingPreset(std::function<void(AudioProcessor *, const AudioPreset&)> callback);
    void AddLFO(std::shared_ptr<LFO> lfo);
    virtual void ClearModulations() {};
    
    AudioBuffer Process(size_t numFrames, const AudioPreset& preset);
    
    virtual void ProcessFrame(AudioFrame& output) = 0;
    void ClearVisited();

    Gain gain;
    Pan pan;
    bool isOn = true;
    float mix = 1.0f;

private:
    void ApplyGainAndPan(AudioFrame& frame); 

    bool m_visited = false;
    std::unique_ptr<AudioBuffer> m_currentResult;

    std::unordered_set<std::shared_ptr<AudioProcessor>> m_children;
    std::unordered_set<std::shared_ptr<LFO>> m_lfos;
    std::optional<std::function<void(AudioProcessor *, const AudioPreset&)>> m_presetCallback;
};

/*
CLASS HIERARCHY:

AudioProcessor (base)

LFO (base)

Generator (base)
    is AudioProcessor

Oscillator
    is Generator
    is LFO
    has Waveform

Waveform (base)
    is Oscillator

Sine/Saw/Sq/...
    is Waveform

Envelope
    is LFO
*/