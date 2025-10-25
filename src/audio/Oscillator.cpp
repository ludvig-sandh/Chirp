// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "Oscillator.hpp"
#include "AudioEngine.hpp"

Voice::Voice(Frequency freq, std::unique_ptr<Waveform> wf, const Envelope& env)
    : freq(freq)
    , m_wf(std::move(wf))
    , m_env(env)
{}

float Voice::GetNextSample() {
    float dt = 1.0 / SAMPLE_RATE;
    float dOffset = dt * freq.GetAbsolute() * std::pow(2.0f, static_cast<float>(m_octave) - 5.0f);
    m_currentPhase += dOffset;

    // Loop back to always be in range [0, 1]
    m_currentPhase -= static_cast<int>(m_currentPhase);

    return m_wf->GetSampleAt(m_currentPhase) * m_env.GetNextSample();
}

void Voice::SetWaveformType(WaveformInfo::Type type) {
    m_wf = Waveform::ConstructWaveform(type);
}

void Voice::SetOctave(int octave) {
    m_octave = octave;
}

bool Oscillator::NoteOn(Frequency freq) {
    if (m_voices.find(freq.GetAbsolute()) != m_voices.end()) {
        // Don't replay a note that is already currently playing
        return false;
    }
    
    Voice v(freq, Waveform::ConstructWaveform(m_waveformType), m_env);
    m_voices.emplace(freq.GetAbsolute(), std::move(v));
    return true;
}

void Oscillator::NoteOff(Frequency freq) {
    m_voices.erase(freq.GetAbsolute());
}

void Oscillator::SetWaveformType(WaveformInfo::Type type) {
    if (type == m_waveformType) {
        return;
    }
    m_waveformType = type;

    // Update all playing notes to use new waveform instead
    for (auto& [_, voice] : m_voices) {
        voice.SetWaveformType(type);
    }
}

// Update the envelope used for note volume
void Oscillator::SetEnvelope(Envelope envelope) {
    m_env = envelope;
}

void Oscillator::SetOctave(int octave) {
    for (auto& [_, voice] : m_voices) {
        voice.SetOctave(octave);
    }
}

void Oscillator::ApplyModulation(float amount, ModulationType modType) {
    if (modType == ModulationType::Pitch) {
        // Modulates the pitch of all voices
        for (auto& [_, voice] : m_voices) {
            voice.freq.AddPitchModulation(amount);
        }
    }else if (modType == ModulationType::Volume) {
        gain.AddModulationLinear(amount);
    }else if (modType == ModulationType::Pan) {
        pan.AddModulation(amount);
    }
}

void Oscillator::ClearModulationsImpl() {
    for (auto& [_, voice] : m_voices) {
        voice.freq.ClearModulations();
    }
    gain.ClearModulations();
    pan.ClearModulations();
}

float Oscillator::GetNextSample() {
    float sample = 0.0f;
    for (auto& [_, voice] : m_voices) {
        sample += voice.GetNextSample();
    }
    return sample;
}