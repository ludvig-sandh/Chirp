// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/generator/Oscillator.hpp"
#include "audio/engine/AudioEngine.hpp"

namespace Audio::Generator {

void Oscillator::NoteOn(Audio::Core::Note note) {
    CleanUpDeadNotes(); // Regularly remove notes that have gone silent

    Voice v(note, Audio::Core::Waveform::ConstructWaveform(m_waveformType), m_env);
    m_voices.push_back(std::move(v));
}

void Oscillator::NoteOff(Audio::Core::Note note) {
    for (auto& v : m_voices) {
        if (v.note == note) {
            v.Release(); // Simply release all notes with the same frequency
        }
    }
}

void Oscillator::SetWaveformType(Audio::Core::WaveformInfo::Type type) {
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
void Oscillator::SetEnvelope(Audio::Modulation::Envelope envelope) {
    m_env = envelope;
}

void Oscillator::SetOctave(int octave) {
    for (auto& voice : m_voices) {
        voice.SetOctave(octave);
    }
}

void Oscillator::ApplyModulation(float amount, Audio::Modulation::Type modType) noexcept {
    if (modType == Audio::Modulation::Type::Pitch) {
        // Modulates the pitch of all voices
        for (auto& voice : m_voices) {
            voice.freq.AddPitchModulation(amount);
        }
    }else if (modType == Audio::Modulation::Type::Volume) {
        gain.AddModulationLinear(amount);
    }else if (modType == Audio::Modulation::Type::Pan) {
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

} // namespace Audio::Generator
