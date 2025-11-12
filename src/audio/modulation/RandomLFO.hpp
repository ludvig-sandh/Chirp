// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/modulation/LFO.hpp"
#include "audio/core/Frequency.hpp"

namespace Audio::Modulation {

class RandomLFO: public LFO {
public:
    RandomLFO(Audio::Core::Frequency freq = Audio::Core::Frequency(1.0f));

    void SetFrequency(Audio::Core::Frequency freq);

    float GetNextSample() override;

private:
    float GenerateRandValueHelper();

    // Simple cubic Hermite interpolation between y0 and y1
    // t = fraction [0,1]
    // Optional tangents m0 and m1; default to 0 for simple smoothness
    float CubicInterpolate(float y0, float y1, float t, float m0 = 0.0, float m1 = 0.0);

    float m_lastRandValue;
    float m_nextRandValue;
    int m_sampleCount = 0;
    int m_numSamplesPerPeriod;
};

} // namespace Audio::Modulation
