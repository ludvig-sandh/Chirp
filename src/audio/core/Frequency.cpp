// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include <algorithm>

#include "audio/core/Frequency.hpp"
#include "audio/engine/AudioEngine.hpp"

namespace Audio::Core {

Frequency::Frequency(float hertz) noexcept
    : m_hertz(hertz)
{}

Frequency::Frequency(Note note) noexcept
    : m_hertz(Frequency::ConvertNoteToHz(note))
{}

void Frequency::SetFrequency(float hertz) noexcept {
    m_hertz = hertz;
}

void Frequency::SetPitch(float semitones) noexcept {
    m_pitchBase = semitones;
}

void Frequency::ClearModulations() noexcept {
    m_pitchModulation = 0.0;
}

void Frequency::AddPitchModulation(float semitones) noexcept {
    m_pitchModulation += semitones;
}

float Frequency::GetAbsolute() const noexcept {
    // The maximum pitch we should support is half the sample rate (Nyquist theorem)
    // Otherwise we'd get foldback aliasing.
    float actual = m_hertz * std::exp2((m_pitchBase + m_pitchModulation) / Note::NOTES_PER_OCTAVE);
    return std::min(actual, static_cast<float>(Audio::Engine::Constants::SAMPLE_RATE / 2.0));
}

float Frequency::ConvertNoteToHz(Note note) noexcept {
    static const float A5 = 440.0f;
    static const float A0 = A5 / 32.0f; // Divide by 2 five times to go down five octaves
    static const float numKeysFromCToA = 9;
    
    // Go down from A to C
    static const float C0 = A0 * std::exp2(-numKeysFromCToA / 12.0f); // C0 = A0 * 2^(-9/12)

    // keyFactor = 2^(key/12)
    const float keyFactor = std::exp2(static_cast<float>(note.key) / 12.0f);
    
    // octaveFactor = 2^octave
    const float octaveFactor = std::exp2(static_cast<float>(note.octave));

    // Compute relative to C0, because that's what note {0, 0} represents
    const float hz = C0 * keyFactor * octaveFactor;
    return hz;
}

} // namespace Audio::Core
