// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/modulation/LFO.hpp"
#include "audio/core/Frequency.hpp"
#include "audio/core/Waveform.hpp"

namespace Audio::Modulation {

class PeriodicLFO: public LFO {
public:
    PeriodicLFO(
        Audio::Core::WaveformInfo::Type waveformType = Audio::Core::WaveformInfo::Type::Saw,
        Audio::Core::Frequency frequency = Audio::Core::Frequency(1.0f)
    );

    // Update the waveform used by this oscillator
    void SetWaveformType(Audio::Core::WaveformInfo::Type type);
    
    void SetFrequency(Audio::Core::Frequency frequency);

    float GetNextSample() override;

private:
    std::unique_ptr<Audio::Core::Waveform> m_waveform;
    Audio::Core::Frequency m_frequency;
    float m_currentPhase = 0.0f;
};

} // namespace Audio::Modulation
