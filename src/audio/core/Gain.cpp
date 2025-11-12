// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/core/Gain.hpp"

#include <cmath>
#include <algorithm>

void Gain::SetLinear(float linear) {
    m_targetLinear = std::clamp(linear, MIN_LINEAR, MAX_LINEAR);
}

void Gain::SetDecibels(float dB) {
    float clamped = std::clamp(dB, MIN_DECIBELS, MAX_DECIBELS);
    float linear = std::pow(10.0f, clamped / 20.0f);
    m_targetLinear = linear;
}

void Gain::AddModulationLinear(float linearMod) {
    m_modulation += linearMod;
}

void Gain::ClearModulations() {
    m_modulation = 0.0f;
}

float Gain::Apply(float sample) {
    float target = std::clamp(m_targetLinear + m_modulation, MIN_LINEAR, MAX_LINEAR);
    m_currentLinear += (target - m_currentLinear) * ALPHA;
    return m_currentLinear * sample;
}

AudioFrame Gain::Apply(const AudioFrame& frame) {
    float target = std::clamp(m_targetLinear + m_modulation, MIN_LINEAR, MAX_LINEAR);
    m_currentLinear += (target - m_currentLinear) * ALPHA;
    return m_currentLinear * frame;
}