// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/core/Note.hpp"
#include <algorithm>
#include <cmath>

class Frequency {
public:
    explicit Frequency(float hertz);
    Frequency(Note note);

    void SetFrequency(float hertz);
    void SetPitch(float semitones);

    void ClearModulations();
    void AddPitchModulation(float semitones);

    float GetAbsolute() const;

private:
    static float ConvertNoteToHz(Note note);

    float m_hertz = 440;
    float m_pitchBase = 0.0; // In semitones (can be negative)
    float m_pitchModulation = 0.0; // In semitones 
    static inline float m_notesPerOctave = 12.0;
};