// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

enum class Key {
    C = 0,
    Cs,
    D,
    Ds,
    E,
    F,
    Fs,
    G,
    Gs,
    A,
    As,
    B
};

struct Note {
    Key key;
    int octave;

    static const int KEYS_PER_OCTAVE = 12;

    Note(Key key, int octave = 5) : key(key), octave(octave) {
        if (octave > 13 || octave < 0) {
            throw std::invalid_argument("Cannot create a note from octave " + std::to_string(octave) + ". Octave must be in range [0, 13].");
        }
    }

    // Order notes first by octave, then by key
    bool operator<(const Note& other) const {
        return (octave < other.octave) || (octave == other.octave && key < other.key);
    }

    // Order notes first by octave, then by key
    bool operator<=(const Note& other) const {
        return *this < other || *this == other;
    }

    bool operator==(const Note& other) const {
        return octave == other.octave && key == other.key;
    }

    // Returns the number of notes from this to the other.
    // If other is higher in pitch than this, returns a negative value.
    int operator-(const Note& other) const {
        return KEYS_PER_OCTAVE * (octave - other.octave) + static_cast<int>(key) - static_cast<int>(other.key);
    }

    // Returns the next key
    Note& operator++() {
        if (key == Key::B) { // Octave changes at B->C
            key = Key::C;
            octave++;
        }else {
            key = static_cast<Key>(static_cast<int>(key) + 1);
        }
        return *this;
    }

    // Returns true if the note is a black key
    bool IsBlackKey() const {
        switch (key) {
            case Key::As:
            case Key::Cs:
            case Key::Ds:
            case Key::Fs:
            case Key::Gs:
                return true;
            default:
                return false;
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