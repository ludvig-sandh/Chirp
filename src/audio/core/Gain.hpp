// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/engine/AudioFrame.hpp"

namespace Audio::Core {

// Keeps track of gain values. When updating gain, it needs to change smoothly rather than jump in value.
// The Gain class takes care of this.
class Gain {
public:
    // Updates the target gain with a value between 0 and 1
    void SetLinear(float linear) noexcept;

    // Updates the target gain with a value between -100.0 and 0.0 decibels
    void SetDecibels(float dB) noexcept;

    // Adds a modulation on the gain (linear addition)
    void AddModulationLinear(float linearMod) noexcept;

    // Removes all modulations on this object
    void ClearModulations() noexcept;

    // Applies the current gain and updates it towards the target gain.
    // Should be called once for each sample.
    float Apply(float sample) noexcept;
    Audio::Engine::AudioFrame Apply(const Audio::Engine::AudioFrame& frame) noexcept;

private:
    float m_currentLinear = 0.0;
    float m_targetLinear = 1.0;
    float m_modulation = 0.0f; // Adds to m_targetLinear

    static constexpr float MAX_LINEAR = 1.0f;
    static constexpr float MIN_LINEAR = 0.0f;
    static constexpr float MAX_DECIBELS = 0.0f;
    static constexpr float MIN_DECIBELS = -100.0f;

    // The rate at which currentGain should reach targetGain
    // Eg. 0.01 would mean it closes in by 1% each sample.
    static constexpr float ALPHA = 0.01;
};

} // namespace Audio::Core
