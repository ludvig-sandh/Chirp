#include "Oscillator.hpp"
#include "AudioEngine.hpp"

float Oscillator::GetNextSample() {
    float dt = 1.0 / SAMPLE_RATE;
    m_timeSinceStart += dt;
    float dOffset = dt * frequency.GetAbsolute();
    m_currentOffset += dOffset;

    // Loop back to always be in range [0, 1]
    m_currentOffset -= static_cast<int>(m_currentOffset);
    
    return m_waveform->GetSampleAt(m_currentOffset);
}