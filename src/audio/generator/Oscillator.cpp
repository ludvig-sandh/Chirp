// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/generator/Oscillator.hpp"
#include "audio/engine/AudioEngine.hpp"

void Oscillator::NoteOn(Note note) {
    CleanUpDeadNotes(); // Regularly remove notes that have gone silent

    Voice v(note, Waveform::ConstructWaveform(m_waveformType), m_env);
    m_voices.push_back(std::move(v));
}

void Oscillator::NoteOff(Note note) {
    for (auto& v : m_voices) {
        if (v.note == note) {
            v.Release(); // Simply release all notes with the same frequency
        }
    }
}

void Oscillator::SetWaveformType(WaveformInfo::Type type) {
    if (type == m_waveformType) {
        return;
    }
    m_waveformType = type;

    // Update all playing notes to use new waveform instead
    for (auto& voice : m_voices) {
        voice.SetWaveformType(type);
    }
}

// Update the envelope used for note volume
void Oscillator::SetEnvelope(Envelope envelope) {
    m_env = envelope;
}

void Oscillator::SetOctave(int octave) {
    for (auto& voice : m_voices) {
        voice.SetOctave(octave);
    }
}

void Oscillator::ApplyModulation(float amount, ModulationType modType) noexcept {
    if (modType == ModulationType::Pitch) {
        // Modulates the pitch of all voices
        for (auto& voice : m_voices) {
            voice.freq.AddPitchModulation(amount);
        }
    }else if (modType == ModulationType::Volume) {
        gain.AddModulationLinear(amount);
    }else if (modType == ModulationType::Pan) {
        pan.AddModulation(amount);
    }
}

void Oscillator::ClearModulationsImpl() noexcept {
    for (auto& voice : m_voices) {
        voice.freq.ClearModulations();
    }
    gain.ClearModulations();
    pan.ClearModulations();
}

float Oscillator::GetNextSample() {
    float sample = 0.0f;
    for (auto& voice : m_voices) {
        sample += voice.GetNextSample();
    }
    return sample;
}

void Oscillator::CleanUpDeadNotes() {
    std::erase_if(m_voices, [](auto const& voice){
        return voice.IsDead(); 
    });
}