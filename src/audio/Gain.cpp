// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "Gain.hpp"

#include <cmath>
#include <algorithm>

void Gain::SetLinear(float linear) {
    m_targetLinear = std::clamp(linear, s_minLinear, s_maxLinear);
}

void Gain::SetDecibels(float dB) {
    float clamped = std::clamp(dB, s_minDecibels, s_maxDecibels);
    float linear = std::pow(10.0f, clamped / 20.0f);
    m_targetLinear = linear;
}

float Gain::GetLinear() const {
    return m_currentLinear;
}

float Gain::GetDecibels() const {
    return 20.0f * std::log10(std::max(m_currentLinear, 1e-5f)); // Avoid log(0)
}

void Gain::AddModulationLinear(float linearMod) {
    m_modulation += linearMod;
}

void Gain::ClearModulations() {
    m_modulation = 0.0f;
}

float Gain::Apply(float sample) {
    float target = std::clamp(m_targetLinear + m_modulation, s_minLinear, s_maxLinear);
    m_currentLinear += (target - m_currentLinear) * s_alpha;
    return m_currentLinear * sample;
}

AudioFrame Gain::Apply(const AudioFrame& frame) {
    float target = std::clamp(m_targetLinear + m_modulation, s_minLinear, s_maxLinear);
    m_currentLinear += (target - m_currentLinear) * s_alpha;
    return m_currentLinear * frame;
}