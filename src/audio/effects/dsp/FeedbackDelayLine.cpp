// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/engine/AudioEngine.hpp"
#include "audio/effects/dsp/FeedbackDelayLine.hpp"
#include <cmath>

FeedbackDelayLine::FeedbackDelayLine(DSP::Seconds delayTime, float feedback)
    : m_buffer(BUFFER_SIZE, 0.0f)
    , m_writeIndex(0)
    , m_delaySamples(0)
    , m_feedback(0.5f)
{
    SetDelayTime(delayTime);
    SetFeedback(feedback);
}

void FeedbackDelayLine::SetDelayTime(DSP::Seconds delayTime) noexcept {
    // Clamp to available buffer size
    float clamped = std::clamp(delayTime.count(), MIN_DELAY_SEC.count(), static_cast<float>(BUFFER_SIZE - 1) / SAMPLE_RATE);
    m_delaySamples = static_cast<int>(clamped * SAMPLE_RATE);
}

void FeedbackDelayLine::SetFeedback(float feedback) noexcept {
    m_feedback = std::clamp(feedback, MIN_FEEDBACK, MAX_FEEDBACK);
}

float FeedbackDelayLine::Process(float input) noexcept {
    const int readIndex = (m_writeIndex + BUFFER_SIZE - m_delaySamples) % BUFFER_SIZE;
    const float delayed = m_buffer[readIndex];

    // Write new sample (input + delayed * feedback)
    m_buffer[m_writeIndex] = input + delayed * m_feedback;

    // Advance write index (circular buffer)
    m_writeIndex = (m_writeIndex + 1) % BUFFER_SIZE;

    return delayed;
}

void FeedbackDelayLine::Clear() {
    std::fill(m_buffer.begin(), m_buffer.end(), 0.0f);
    m_writeIndex = 0;
}
