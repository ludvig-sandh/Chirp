// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "MidiInput.hpp"
#include "gui/windows/KeyboardWindow.hpp"

MidiInput::MidiInput() {
    m_midiIn.ignoreTypes(false, false, false);
}

MidiInput::~MidiInput() {
    m_midiIn.closePort();
}

void MidiInput::OpenDefaultPort() {
    int ports = static_cast<int>(m_midiIn.getPortCount());

    // No ports available
    if (ports == 0) {
        if (m_hadDevice) {
            std::cout << "[MIDI] Device disconnected.\n";
        }
        m_hadDevice = false;
        m_portOpen = false;
        m_currentPort = -1;
        return;
    }

    // If already connected and the same port still exists, do nothing
    if (m_portOpen && m_currentPort >= 0 && m_currentPort < ports) {
        std::string currentName;
        try {
            currentName = m_midiIn.getPortName(m_currentPort);
        }catch (...) {
            // The port index is no longer valid → treat as disconnected.
            std::cout << "[MIDI] Device disconnected.\n";
            m_midiIn.closePort();
            m_portOpen = false;
            m_hadDevice = false;
            m_currentPort = -1;
            return;
        }

        if (currentName.find("Midi Through") == std::string::npos) {
            // Still valid => nothing to do
            return;
        }
    }

    // Try to find a hardware device (skip Midi Through since it doesn't provide any input. Exists on linux sometimes)
    for (int i = 0; i < ports; ++i) {
        std::string name = m_midiIn.getPortName(i);

        if (name.find("Midi Through") != std::string::npos) {
            continue;
        }

        try {
            // Close old port before opening a new one
            if (m_portOpen) {
                m_midiIn.closePort();
            }

            m_midiIn.openPort(i);
            std::cout << "[MIDI] Connected to input port: " << name << "\n";

            m_portOpen = true;
            m_hadDevice = true;
            m_currentPort = i;
            return;
        }catch (RtMidiError& e) {
            std::cerr << "[MIDI] Failed to open port " << i << " (" << name << "): "
                      << e.getMessage() << "\n";
        }
    }

    // No usable device found
    if (m_hadDevice) {
        std::cout << "[MIDI] Device disconnected.\n";
    }

    m_midiIn.closePort();
    m_portOpen = false;
    m_hadDevice = false;
    m_currentPort = -1;
}

std::set<Audio::Core::Note> MidiInput::GetPressedNotes() {
    OpenDefaultPort();

    if (!m_portOpen) {
        m_pressedNotes.clear();
        return m_pressedNotes;
    }

    std::vector<unsigned char> msg;
    double timestamp;

    try {
        while (true) {
            msg.clear();
            timestamp = m_midiIn.getMessage(&msg);
            if (msg.empty()) break;

            const unsigned char status = msg[0];
            const unsigned char data1  = std::ssize(msg) > 1 ? msg[1] : 0; // note number
            const unsigned char data2  = std::ssize(msg) > 2 ? msg[2] : 0; // velocity
            const int command = status & 0xF0;

            Audio::Core::Note note = MidiNoteToNote(static_cast<int>(data1));

            // Don't allow playing notes outside the keyboard range
            if (note < GUI::Window::KeyboardWindow::FIRST_NOTE || note > GUI::Window::KeyboardWindow::LAST_NOTE) {
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
Audio::Core::Note MidiInput::MidiNoteToNote(int midiNumber) {
    int octave = midiNumber / Audio::Core::Note::NOTES_PER_OCTAVE - 1;
    int keyIndex = midiNumber % Audio::Core::Note::NOTES_PER_OCTAVE;
    return Audio::Core::Note(static_cast<Audio::Core::Key>(keyIndex), octave);
}