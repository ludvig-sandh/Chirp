// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "Gain.hpp"
#include "AudioFrame.hpp"

class Pan {
public:
    void Set(float pan);
    void AddModulation(float modulation);
    void ClearModulations();
    AudioFrame Apply(const AudioFrame& frame);
private:
    float m_pan = 0.5f; // Center
    float m_modulation = 0.0f;
    Gain m_rightGain;
    Gain m_leftGain;
};
