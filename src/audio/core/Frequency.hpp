// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <algorithm>
#include <cmath>
#include <format>

enum Key {
    A = 0,
    As,
    B,
    C,
    Cs,
    D,
    Ds,
    E,
    F,
    Fs,
    G,
    Gs
};

struct Note {
    Key key;
    int octave;

    Note(Key key, int octave = 5) : key(key), octave(octave) {
        if (octave > 13 || octave < 1) {
            throw std::invalid_argument(std::format("Cannot create a note from octave {}. Octave must be in range [1, 13].", octave));
        }
    }
};

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