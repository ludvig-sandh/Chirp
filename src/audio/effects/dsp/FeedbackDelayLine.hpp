// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/effects/dsp/Time.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

// A single-channel delay line building block for different types of delay effects
class FeedbackDelayLine {
public:
    // Constants for safety
    static constexpr DSP::Seconds MIN_DELAY_SEC = DSP::Seconds(0.05f);
    static constexpr DSP::Seconds MAX_DELAY_SEC = DSP::Seconds(10.0f);
    static constexpr float MAX_FEEDBACK = 0.99f;
    static constexpr float MIN_FEEDBACK = 0.01f;

    FeedbackDelayLine(DSP::Seconds delayTime, float feedback);

    // Adjust delay time (seconds), safely clamped to allowed range
    void SetDelayTime(DSP::Seconds delayTime) noexcept;

    // Adjust feedback amount, safely clamped to allowed range
    void SetFeedback(float feedback) noexcept;

    // Process one input sample and return the delayed output (without the input signal)
    float Process(float input) noexcept;

    // Reset the delay line buffer to silence
    void Clear() noexcept;

private:
    static inline const int BUFFER_SIZE = static_cast<int>(std::ceil(MAX_DELAY_SEC.count() * 44100)) + 1; // TODO: Provide sample rate at runtime

    std::vector<float> m_buffer;
    int m_writeIndex = 0;
    int m_delaySamples = 0;
    float m_feedback = 0.5f;
};
