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
#include "ModulationMatrix.hpp"

// Represents a node in a tree showing how audio is routed throughout the engine.
class AudioProcessor {
public:
    AudioProcessor() = default;
    virtual ~AudioProcessor() = default;

    void AddChild(std::shared_ptr<AudioProcessor> child);
    
    void ClearModulations();
    virtual void ClearModulationsImpl() {};
    virtual void ApplyModulation(float amount, ModulationType modType);

    AudioFrame GenerateFrame(const AudioPreset& preset);
    
    virtual void ProcessFrame(AudioFrame& output) = 0;
    void ClearVisited();

    Gain gain;
    Pan pan;
    bool isOn = true;
    float mix = 1.0f;

private:
    void ApplyGainAndPan(AudioFrame& frame); 

    bool m_visited = false;
    AudioFrame m_cachedResult;

    std::unordered_set<std::shared_ptr<AudioProcessor>> m_children;
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