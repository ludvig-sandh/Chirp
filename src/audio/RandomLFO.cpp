// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "RandomLFO.hpp"
#include "AudioEngine.hpp"

RandomLFO::RandomLFO(Frequency freq) {
    m_numSamplesPerPeriod = SAMPLE_RATE / freq.GetAbsolute();
    m_lastRandValue = GenerateRandValueHelper();
    m_nextRandValue = GenerateRandValueHelper();
}

void RandomLFO::SetFrequency(Frequency freq) {
    m_numSamplesPerPeriod = SAMPLE_RATE / freq.GetAbsolute();
}

float RandomLFO::GetNextSample() {
    float t = static_cast<float>(m_sampleCount) / m_numSamplesPerPeriod;
    float toReturn = CubicInterpolate(m_lastRandValue, m_nextRandValue, t);

    m_sampleCount++;
    if (m_sampleCount >= m_numSamplesPerPeriod) {
        m_sampleCount = 0;
        m_lastRandValue = m_nextRandValue;
        m_nextRandValue = GenerateRandValueHelper();
    }

    return toReturn;
}

float RandomLFO::GenerateRandValueHelper() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float RandomLFO::CubicInterpolate(float y0, float y1, float t, float m0, float m1) {
    float t2 = t * t;
    float t3 = t2 * t;

    float h00 = 2*t3 - 3*t2 + 1;
    float h10 = t3 - 2*t2 + t;
    float h01 = -2*t3 + 3*t2;
    float h11 = t3 - t2;

    return h00*y0 + h10*m0 + h01*y1 + h11*m1;
}