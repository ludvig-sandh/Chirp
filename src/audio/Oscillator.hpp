// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "Frequency.hpp"
#include "Generator.hpp"
#include "Waveform.hpp"
#include "Envelope.hpp"
#include "LFO.hpp"

#include <unordered_map>

class Voice {
public:
    Voice(Frequency freq, std::unique_ptr<Waveform> wf, const Envelope& env);
    float GetNextSample();
    void SetWaveformType(WaveformInfo::Type type);
    void SetOctave(int octave);

    Frequency freq;

private:
    std::unique_ptr<Waveform> m_wf;
    Envelope m_env;
    int m_octave = 5;
    float m_currentPhase = 0.0f;
};

class Oscillator final : public Generator {
public:
    explicit Oscillator(WaveformInfo::Type type = WaveformInfo::Type::Saw) : m_waveformType(type) {}

    // Start a new voice. Returns true if started new note, and false if a note was already playing.
    bool NoteOn(Frequency freq);

    // Stop an existing voice
    void NoteOff(Frequency freq);

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
    WaveformInfo::Type m_waveformType;
    Envelope m_env;
    std::unordered_map<float, Voice> m_voices;
    int octave = 5;
};