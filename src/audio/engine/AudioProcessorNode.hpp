// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <functional>
#include <optional>

#include "audio/core/Frequency.hpp"
#include "audio/engine/AudioBackend.hpp"
#include "audio/modulation/LFO.hpp"
#include "audio/core/Gain.hpp"
#include "audio/core/Pan.hpp"
#include "audio/modulation/ModulationMatrix.hpp"

// Represents a node in a tree showing how audio is routed throughout the engine.
class AudioProcessorNode {
public:
    AudioProcessorNode() = default;
    virtual ~AudioProcessorNode() noexcept = default;

    void AddChild(std::shared_ptr<AudioProcessorNode> child);
    
    void ClearModulations() noexcept;
    virtual void ClearModulationsImpl() noexcept {};
    virtual void ApplyModulation(float amount, ModulationType modType) noexcept;

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

    std::unordered_set<std::shared_ptr<AudioProcessorNode>> m_children;
};

/*
CLASS HIERARCHY:

AudioProcessorNode (base)

LFO (base)

Generator (base)
    is AudioProcessorNode

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