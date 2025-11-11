// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

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

    static const int NOTES_PER_OCTAVE = 12;

    Note(Key key, int octave = 5);

    // Order notes first by octave, then by key
    bool operator<(const Note& other) const;

    // Order notes first by octave, then by key
    bool operator<=(const Note& other) const;

    bool operator==(const Note& other) const;

    // Returns the number of notes from this to the other.
    // If other is higher in pitch than this, returns a negative value.
    int operator-(const Note& other) const;

    // Returns the next key
    Note& operator++();

    // Returns true if the note is a black key
    bool IsBlackKey() const;
};
