// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/core/Note.hpp"
#include <algorithm>
#include <cmath>

namespace Audio::Core {

class Frequency {
public:
    explicit Frequency(float hertz) noexcept;
    Frequency(Note note) noexcept;

    // Sets the base frequency.
    void SetFrequency(float hertz) noexcept;

    // Sets the pitch in semitones, relative to the base frequency
    void SetPitch(float semitones) noexcept;

    void ClearModulations() noexcept;

    // Adds pitch modulation in semitones, which is added to the relative pitch from the base frequency
    void AddPitchModulation(float semitones) noexcept;

    // Get the frequency accounting for any pitch and modulation relative to base frequency
    float GetAbsolute() const noexcept;

private:
    static float ConvertNoteToHz(Note note) noexcept;

    float m_hertz = 440;
    float m_pitchBase = 0.0; // In semitones (can be negative)
    float m_pitchModulation = 0.0; // In semitones 
};

} // namespace Audio::Core
