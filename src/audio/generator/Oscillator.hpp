// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "core/Frequency.hpp"
#include "Generator.hpp"
#include "core/Waveform.hpp"
#include "modulation/Envelope.hpp"
#include "modulation/LFO.hpp"

#include <vector>

class Voice {
public:
    Voice(Note note, std::unique_ptr<Waveform> wf, const Envelope& env);
    float GetNextSample();
    void SetWaveformType(WaveformInfo::Type type);
    void SetOctave(int octave);
    void Release(); // Tells the envelope to go into "release" state to fade out the note
    bool IsDead() const; // Returns true if the note is quiet indefinitely from this point and onward

    Note note;
    Frequency freq;
private:
    std::unique_ptr<Waveform> m_wf;
    Envelope m_env;
    float m_currentPhase = 0.0f;
};

class Oscillator final : public Generator {
public:
    explicit Oscillator(WaveformInfo::Type type = WaveformInfo::Type::Saw) : m_waveformType(type) {}

    // Start a new voice
    void NoteOn(Note note);

    // Stop an existing voice
    void NoteOff(Note note);

    // Update the waveform used by this oscillator
    void SetWaveformType(WaveformInfo::Type type);

    // Update the envelope used for note volume
    void SetEnvelope(Envelope envelope);

    // Updates the octave used for the root note A5 at 440Hz (5 is default, per definition)
    void SetOctave(int octave);

    void ApplyModulation(float amount, ModulationType modType) override;

    // Clears all modulations accumulated from LFOs in the last frame so they can modulate the next one
    virtual void ClearModulationsImpl() override;

    // Returns the next sample for this oscillator. Must be called once every frame or it will become desynched.
    float GetNextSample() override;
    
private:
    // Remove voices lazily which allows them to play the "release" of a note
    void CleanUpDeadNotes();

    WaveformInfo::Type m_waveformType;
    Envelope m_env;
    std::vector<Voice> m_voices;
    int octave = 5;
};