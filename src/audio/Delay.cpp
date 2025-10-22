#include "Delay.hpp"
#include "AudioEngine.hpp"

Delay::Delay(float delaySeconds)
{
    SetDelay(delaySeconds);
}

// Set delay in seconds
void Delay::SetDelay(float delaySeconds) {
    m_delayInSeconds = delaySeconds;
    int newDelaySamples = static_cast<int>(std::round(m_delayInSeconds * SAMPLE_RATE));
    m_buffer.resize(newDelaySamples + 1, 0.0f); // +1 for safety margin
    m_writeIndex = 0;
}

// Process one sample at a time
float Delay::Process(float input) {
    if (m_buffer.empty()) {
        return input;
    }

    int delaySamples = static_cast<int>(std::round(m_delayInSeconds * SAMPLE_RATE));
    int readIndex = m_writeIndex - delaySamples;
    if (readIndex < 0) {
        readIndex += static_cast<int>(m_buffer.size());
    }

    float delayedSample = m_buffer[readIndex];

    m_buffer[m_writeIndex] = input;  // store current input in delay buffer
    m_writeIndex = (m_writeIndex + 1) % m_buffer.size();  // wrap around

    return delayedSample;
}
