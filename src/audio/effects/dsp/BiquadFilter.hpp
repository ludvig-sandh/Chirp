// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/engine/AudioProcessorNode.hpp"

namespace Audio::Effects::DSP {

// A simple biquad filter for a single channel.
// https://arachnoid.com/BiQuadDesigner/index.html
// For an overview of how to compute the coefficients for different types of filters, I used
// https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html?utm_source=chatgpt.com
class BiquadFilter {
public:
    void SetCoefficients(float b0, float b1, float b2, float a1, float a2) noexcept;

    // Uses x as input to the filter and progresses to the next step, providing the output y
    float Step(float x) noexcept;

private:
    // Just placeholder values
    float m_b0 = 1.0f;
    float m_b1 = 1.0f;
    float m_b2 = 1.0f;
    float m_a1 = 1.0f;
    float m_a2 = 1.0f;

    float m_y1 = 0.0f; // y[n-1]
    float m_y2 = 0.0f; // y[n-2]
    float m_x1 = 0.0f; // x[n-1]
    float m_x2 = 0.0f; // x[n-2]
};

} // namespace Audio::Effects::DSP
