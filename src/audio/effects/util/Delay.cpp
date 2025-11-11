// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/effects/util/Delay.hpp"
#include "audio/engine/AudioEngine.hpp"

Delay::Delay(float delaySeconds)
{
    SetDelay(delaySeconds);
}

// Set delay in seconds
void Delay::SetDelay(float delaySeconds) {
    m_delayInSeconds = delaySeconds;
    int newDelaySamples = static_cast<int>(std::round(m_delayInSeconds * SAMPLE_RATE)) + 1; // +1 for safety margin
    if (std::ssize(m_buffer) != newDelaySamples) {
        m_buffer.resize(newDelaySamples, 0.0f);
        m_writeIndex = 0;
    }
}

// Process one sample at a time
float Delay::Process(float input) {
    if (m_buffer.empty()) {
        return input;
    }

    int delaySamples = static_cast<int>(std::round(m_delayInSeconds * SAMPLE_RATE));
    int readIndex = m_writeIndex - delaySamples;
    if (readIndex < 0) {
        readIndex += std::ssize(m_buffer);
    }

    float delayedSample = m_buffer[readIndex];

    m_buffer[m_writeIndex] = input; // store current input in delay buffer
    m_writeIndex = (m_writeIndex + 1) % std::ssize(m_buffer); // wrap around

    return delayedSample;
}
