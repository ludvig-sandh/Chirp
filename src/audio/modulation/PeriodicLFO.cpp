// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/modulation/PeriodicLFO.hpp"
#include "audio/engine/AudioEngine.hpp"

namespace Audio::Modulation {

PeriodicLFO::PeriodicLFO(Audio::Core::WaveformInfo::Type type, Audio::Core::Frequency frequency)
    : m_waveform(Audio::Core::Waveform::ConstructWaveform(type))
    , m_frequency(frequency)
{}

void PeriodicLFO::SetWaveformType(Audio::Core::WaveformInfo::Type type) {
    m_waveform = Audio::Core::Waveform::ConstructWaveform(type);
}
    
void PeriodicLFO::SetFrequency(Audio::Core::Frequency frequency) {
    m_frequency = frequency;
}

float PeriodicLFO::GetNextSample() {
    float dt = 1.0 / Audio::Engine::Constants::SAMPLE_RATE;
    float dOffset = dt * m_frequency.GetAbsolute();
    m_currentPhase += dOffset;

    // Loop back to always be in range [0, 1]
    m_currentPhase -= static_cast<int>(m_currentPhase);

    return m_waveform->GetSampleAt(m_currentPhase) / 2.0f + 0.5f; // Shift to range [0, 1] for LFOs
}

} // namespace Audio::Modulation
