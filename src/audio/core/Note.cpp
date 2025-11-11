// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/core/Note.hpp"

#include <stdexcept>
#include <string>

Note::Note(Key key, int octave) : key(key), octave(octave) {}

// Order notes first by octave, then by key
bool Note::operator<(const Note& other) const {
    return (octave < other.octave) || (octave == other.octave && key < other.key);
}

bool Note::operator>(const Note& other) const {
    return (octave > other.octave) || (octave == other.octave && key > other.key);
}

// Order notes first by octave, then by key
bool Note::operator<=(const Note& other) const {
    return *this < other || *this == other;
}

bool Note::operator==(const Note& other) const {
    return octave == other.octave && key == other.key;
}

// Returns the number of notes from this to the other.
// If other is higher in pitch than this, returns a negative value.
int Note::operator-(const Note& other) const {
    return NOTES_PER_OCTAVE * (octave - other.octave) + static_cast<int>(key) - static_cast<int>(other.key);
}

// Returns the next key
Note& Note::operator++() {
    if (key == Key::B) { // Octave changes at B->C
        key = Key::C;
        octave++;
    }else {
        key = static_cast<Key>(static_cast<int>(key) + 1);
    }
    return *this;
}

// Returns true if the note is a black key
bool Note::IsBlackKey() const {
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
