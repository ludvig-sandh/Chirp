#include "PeriodicLFO.hpp"
#include "AudioEngine.hpp"

PeriodicLFO::PeriodicLFO(WaveformInfo::Type type, Frequency frequency)
    : m_waveform(Waveform::ConstructWaveform(type))
    , m_frequency(frequency)
{}

void PeriodicLFO::SetWaveformType(WaveformInfo::Type type) {
    m_waveform = Waveform::ConstructWaveform(type);
}
    
void PeriodicLFO::SetFrequency(Frequency frequency) {
    m_frequency = frequency;
}

float PeriodicLFO::GetNextSample() {
    float dt = 1.0 / SAMPLE_RATE;
    float dOffset = dt * m_frequency.GetAbsolute();
    m_currentPhase += dOffset;

    // Loop back to always be in range [0, 1]
    m_currentPhase -= static_cast<int>(m_currentPhase);

    return m_waveform->GetSampleAt(m_currentPhase) / 2.0f + 0.5f; // Shift to range [0, 1] for LFOs
}