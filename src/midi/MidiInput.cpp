// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "MidiInput.hpp"
#include "gui/Keyboard.hpp"

MidiInput::MidiInput() {
    m_midiIn.ignoreTypes(false, false, false);
}

MidiInput::~MidiInput() {
    m_midiIn.closePort();
}

void MidiInput::OpenDefaultPort() {
    unsigned int ports = m_midiIn.getPortCount();
    if (ports == 0) {
        if (m_hadDevice) {
            std::cout << "[MIDI] Device disconnected.\n";
            m_hadDevice = false;
        }
        m_portOpen = false;
        return;
    }

    try {
        m_midiIn.openPort(0);
        std::cout << "[MIDI] Connected to input port: " << m_midiIn.getPortName(0) << "\n";
        m_portOpen = true;
        m_hadDevice = true;
    } catch (RtMidiError& e) {
        std::cerr << "[MIDI] Failed to open input port: " << e.getMessage() << "\n";
        m_portOpen = false;
    }
}

std::set<Note> MidiInput::GetPressedNotes() {
    // If no port yet, attempt to open automatically
    if (!m_portOpen) {
        OpenDefaultPort();
        if (!m_portOpen) return m_pressedNotes;
    }

    std::vector<unsigned char> msg;
    double timestamp;

    try {
        while (true) {
            msg.clear();
            timestamp = m_midiIn.getMessage(&msg);
            if (msg.empty()) break;

            const unsigned char status = msg[0];
            const unsigned char data1  = msg.size() > 1 ? msg[1] : 0; // note number
            const unsigned char data2  = msg.size() > 2 ? msg[2] : 0; // velocity
            const int command = status & 0xF0;

            Note note = MidiNoteToNote(static_cast<int>(data1));

            // Don't allow playing notes outside the keyboard range
            if (note < Keyboard::FIRST_NOTE || note > Keyboard::LAST_NOTE) {
                continue;
            }

            if (command == 0x90 && data2 > 0) {
                m_pressedNotes.insert(note);
            } else if (command == 0x80 || (command == 0x90 && data2 == 0)) {
                m_pressedNotes.erase(note);
            }
        }
    } catch (RtMidiError& e) {
        std::cerr << "[MIDI] Lost connection to device (" << e.getMessage() << ")\n";
        m_midiIn.closePort();
        m_portOpen = false;
    }

    // Detect unplug after the fact
    if (m_midiIn.getPortCount() == 0 && m_portOpen) {
        std::cout << "[MIDI] Device disconnected.\n";
        m_midiIn.closePort();
        m_portOpen = false;
        m_pressedNotes.clear(); // prevent stuck notes
    }

    return m_pressedNotes;
}

// Converts a MIDI note number (0–127) to a Chirp Note object
Note MidiInput::MidiNoteToNote(int midiNumber) {
    int octave = midiNumber / Note::NOTES_PER_OCTAVE - 1;
    int keyIndex = midiNumber % Note::NOTES_PER_OCTAVE;
    return Note(static_cast<Key>(keyIndex), octave);
}