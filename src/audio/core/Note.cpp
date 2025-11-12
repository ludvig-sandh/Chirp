// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/core/Note.hpp"

#include <stdexcept>
#include <string>

Note::Note(Key key, int octave) noexcept : key(key), octave(octave) {}

bool Note::operator<(const Note& other) const noexcept {
    return (octave < other.octave) || (octave == other.octave && key < other.key);
}

bool Note::operator>(const Note& other) const noexcept {
    return (octave > other.octave) || (octave == other.octave && key > other.key);
}

bool Note::operator<=(const Note& other) const noexcept {
    return *this < other || *this == other;
}

bool Note::operator==(const Note& other) const noexcept {
    return octave == other.octave && key == other.key;
}

int Note::operator-(const Note& other) const noexcept {
    return NOTES_PER_OCTAVE * (octave - other.octave) + static_cast<int>(key) - static_cast<int>(other.key);
}

Note& Note::operator++() noexcept {
    if (key == Key::B) { // Octave changes at B->C
        key = Key::C;
        octave++;
    }else {
        key = static_cast<Key>(static_cast<int>(key) + 1);
    }
    return *this;
}

bool Note::IsBlackKey() const noexcept {
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
