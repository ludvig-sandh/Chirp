#include "DelayLine.hpp"
#include "AudioEngine.hpp"
#include <cmath>

DelayLine::DelayLine(float delayTime, float feedback) {
    Init();
    SetDelayTime(delayTime);
    SetFeedback(feedback);
}

void DelayLine::Init() {
    // Safety clamp
    const float safeMaxDelay = std::max(MAX_DELAY_SEC, MIN_DELAY_SEC);

    m_bufferSize = static_cast<size_t>(std::ceil(safeMaxDelay * SAMPLE_RATE)) + 1;

    m_buffer.assign(m_bufferSize, 0.0f);
    m_writeIndex = 0;
    m_delaySamples = 0;
    m_feedback = 0.5f;
}

void DelayLine::SetDelayTime(float seconds) noexcept{
    // Clamp to available buffer size
    float clamped = std::clamp(seconds, MIN_DELAY_SEC, static_cast<float>(m_bufferSize - 1) / SAMPLE_RATE);
    m_delaySamples = static_cast<size_t>(clamped * SAMPLE_RATE);
}

void DelayLine::SetFeedback(float fb) noexcept {
    m_feedback = std::clamp(fb, 0.0f, MAX_FEEDBACK);
}

float DelayLine::Process(float input) noexcept {
    if (m_bufferSize == 0) {
        return input; // not initialized yet, pass-through
    }

    const size_t readIndex =
        (m_writeIndex + m_bufferSize - m_delaySamples) % m_bufferSize;

    const float delayed = m_buffer[readIndex];

    // Write new sample (input + delayed * feedback)
    m_buffer[m_writeIndex] = input + delayed * m_feedback;

    // Advance write index (circular buffer)
    m_writeIndex = (m_writeIndex + 1) % m_bufferSize;

    return delayed;
}

void DelayLine::Clear() noexcept {
    std::fill(m_buffer.begin(), m_buffer.end(), 0.0f);
    m_writeIndex = 0;
}
