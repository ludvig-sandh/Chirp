// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/core/Gain.hpp"

#include <cmath>
#include <algorithm>

namespace Audio::Core {

void Gain::SetLinear(float linear) noexcept {
    m_targetLinear = std::clamp(linear, MIN_LINEAR, MAX_LINEAR);
}

void Gain::SetDecibels(float dB) noexcept {
    float clamped = std::clamp(dB, MIN_DECIBELS, MAX_DECIBELS);
    float linear = std::pow(10.0f, clamped / 20.0f);
    m_targetLinear = linear;
}

void Gain::AddModulationLinear(float linearMod) noexcept {
    m_modulation += linearMod;
}

void Gain::ClearModulations() noexcept {
    m_modulation = 0.0f;
}

float Gain::Apply(float sample) noexcept {
    float target = std::clamp(m_targetLinear + m_modulation, MIN_LINEAR, MAX_LINEAR);
    m_currentLinear += (target - m_currentLinear) * ALPHA;
    return m_currentLinear * sample;
}

Audio::Engine::AudioFrame Gain::Apply(const Audio::Engine::AudioFrame& frame) noexcept {
    float target = std::clamp(m_targetLinear + m_modulation, MIN_LINEAR, MAX_LINEAR);
    m_currentLinear += (target - m_currentLinear) * ALPHA;
    return m_currentLinear * frame;
}

} // namespace Audio::Core
