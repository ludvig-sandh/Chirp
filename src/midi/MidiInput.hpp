// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <RtMidi.h>
#include <set>
#include <vector>
#include "audio/core/Frequency.hpp" // For Note

// Simple MIDI input wrapper.
// Automatically opens and reopens a MIDI port when available,
// and returns a set of pressed Note values.
class MidiInput {
public:
    MidiInput();
    ~MidiInput();

    // Explicitly try to open the first available MIDI input port, unless a port is already in use
    void OpenDefaultPort();

    // Polls MIDI messages, auto-opens ports if needed, and returns pressed notes
    std::set<Note> GetPressedNotes();

private:
    static Note MidiNoteToNote(int midiNumber);
    
    RtMidiIn m_midiIn;
    std::set<Note> m_pressedNotes;
    bool m_portOpen = false;
    bool m_hadDevice = false;
    int m_currentPort = -1;
};
