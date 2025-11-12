// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/core/Note.hpp"
#include "audio/core/Frequency.hpp"
#include "audio/core/Waveform.hpp"
#include "audio/modulation/Envelope.hpp"
#include <memory>

// Represents one of the voices an oscillator generates. This is needed since we support polyphony (multiple notes playing simultaneously).
class Voice {
public:
    Voice(Note note, std::unique_ptr<Waveform> wf, const Envelope& env);

    // Returns the next sample of this voice
    float GetNextSample();

    // Updates the waveform
    void SetWaveformType(WaveformInfo::Type type);

    // Updates the octave number
    void SetOctave(int octave);
    
    // Tells the envelope to go into "release" state to fade out the note
    void Release();

    // Returns true if the note is quiet indefinitely from this point and onward, safe to delete
    bool IsDead() const;

    Note note;
    Frequency freq;

private:
    std::unique_ptr<Waveform> m_wf;
    Envelope m_env;
    float m_currentPhase = 0.0f;
};