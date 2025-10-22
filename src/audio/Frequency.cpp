// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include <algorithm>

#include "Frequency.hpp"
#include "AudioEngine.hpp"

Frequency::Frequency(float hertz)
    : m_hertz(hertz)
    , m_pitchBase(0.0)
    , m_pitchModulation(0.0)
{}

Frequency::Frequency(Note note)
    : m_hertz(Frequency::ConvertNoteToHz(note))
    , m_pitchBase(0.0)
    , m_pitchModulation(0.0)
{}

void Frequency::SetFrequency(float hertz) {
    m_hertz = hertz;
}

void Frequency::SetPitch(float semitones) {
    m_pitchBase = semitones;
}

void Frequency::ClearModulations() {
    m_pitchModulation = 0.0;
}

void Frequency::AddPitchModulation(float semitones) {
    m_pitchModulation += semitones;
}

float Frequency::GetAbsolute() const {
    // The maximum pitch we should support is half the sample rate (Nyquist theorem)
    // Otherwise we'd get foldback aliasing.
    float actual = m_hertz * std::pow(2.0, (m_pitchBase + m_pitchModulation) / m_notesPerOctave);
    return std::min(actual, static_cast<float>(SAMPLE_RATE / 2.0));
}

float Frequency::ConvertNoteToHz(Note note) {
    float A5 = 440.0f;
    float twelvethSq2 = std::pow(2.0f, 1.0f / 12.0f); // Twelveth square root of two
    float hz = A5 * std::pow(twelvethSq2, note.key) * std::pow(2.0f, static_cast<float>(note.octave - 5));
    return hz;
}