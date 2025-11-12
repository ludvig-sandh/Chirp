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
    float actual = m_hertz * std::pow(2.0, (m_pitchBase + m_pitchModulation) / Note::NOTES_PER_OCTAVE);
    return std::min(actual, static_cast<float>(Audio::Engine::Constants::SAMPLE_RATE / 2.0));
}

float Frequency::ConvertNoteToHz(Note note) noexcept {
    const float A5 = 440.0f;
    static const float twelvethSq2 = std::pow(2.0f, 1.0f / 12.0f); // Twelveth square root of two
    float hz = A5 * std::pow(twelvethSq2, static_cast<float>(note.key)) * std::pow(2.0f, static_cast<float>(note.octave - 5));
    return hz;
}

} // namespace Audio::Core
