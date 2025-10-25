// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "LFO.hpp"
#include "Frequency.hpp"
#include "Waveform.hpp"

class PeriodicLFO: public LFO {
public:
    PeriodicLFO(WaveformInfo::Type waveformType = WaveformInfo::Type::Saw, Frequency frequency = Frequency(1.0f));

    // Update the waveform used by this oscillator
    void SetWaveformType(WaveformInfo::Type type);
    
    void SetFrequency(Frequency frequency);

    float GetNextSample() override;

private:
    std::unique_ptr<Waveform> m_waveform;
    Frequency m_frequency;
    float m_currentPhase = 0.0f;
};