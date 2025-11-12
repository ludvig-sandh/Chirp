// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/effects/dsp/Delay.hpp"
#include "audio/engine/AudioEngine.hpp"

#include <stdexcept>

namespace Audio::Effects::DSP {

Delay::Delay(DSP::Seconds delayTime) {
    SetDelay(delayTime);
}

// Set delay in seconds
void Delay::SetDelay(DSP::Seconds delayTime) {
    if (delayTime < DSP::Seconds(0)) {
        throw std::invalid_argument("Delay::SetDelay: Cannot apply a negative delay time.");
    }

    m_delayTime = delayTime;
    int newDelaySamples = static_cast<int>(std::round(m_delayTime.count() * Audio::Engine::Constants::SAMPLE_RATE)) + 1; // +1 for safety margin
    if (std::ssize(m_buffer) != newDelaySamples) {
        m_buffer.resize(newDelaySamples, 0.0f);
        m_writeIndex = 0;
    }
}

// Process one sample at a time
float Delay::Process(float input) noexcept {
    if (m_buffer.empty()) {
        return input;
    }

    int delaySamples = static_cast<int>(std::round(m_delayTime.count() * Audio::Engine::Constants::SAMPLE_RATE));
    int readIndex = m_writeIndex - delaySamples;
    if (readIndex < 0) {
        readIndex += std::ssize(m_buffer);
    }

    float delayedSample = m_buffer[readIndex];

    m_buffer[m_writeIndex] = input; // store current input in delay buffer
    m_writeIndex = (m_writeIndex + 1) % std::ssize(m_buffer); // wrap around

    return delayedSample;
}

} // namespace Audio::Effects::DSP
