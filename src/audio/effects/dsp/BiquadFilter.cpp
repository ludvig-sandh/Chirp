// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/effects/dsp/BiquadFilter.hpp"

void BiquadFilter::SetCoefficients(float b0, float b1, float b2, float a1, float a2) noexcept {
    m_b0 = b0;
    m_b1 = b1;
    m_b2 = b2;
    m_a1 = a1;
    m_a2 = a2;
}

float BiquadFilter::Step(float x) noexcept {
    // Compute result
    float y = m_b0 * x + m_b1 * m_x1 + m_b2 * m_x2 - m_a1 * m_y1 - m_a2 * m_y2;

    // Update state
    m_y2 = m_y1;
    m_y1 = y;
    m_x2 = m_x1;
    m_x1 = x;

    return y;
}