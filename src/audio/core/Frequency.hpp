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

    // Sets the base frequency.
    void SetFrequency(float hertz);

    // Sets the pitch in semitones, relative to the base frequency
    void SetPitch(float semitones);

    void ClearModulations();

    // Adds pitch modulation in semitones, which is added to the relative pitch from the base frequency
    void AddPitchModulation(float semitones);

    // Get the frequency accounting for any pitch and modulation relative to base frequency
    float GetAbsolute() const;

private:
    static float ConvertNoteToHz(Note note);

    float m_hertz = 440;
    float m_pitchBase = 0.0; // In semitones (can be negative)
    float m_pitchModulation = 0.0; // In semitones 
};