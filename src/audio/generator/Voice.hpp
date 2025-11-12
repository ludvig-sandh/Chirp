// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/core/Note.hpp"
#include "audio/core/Frequency.hpp"
#include "audio/core/Waveform.hpp"
#include "audio/modulation/Envelope.hpp"
#include <memory>

namespace Audio::Generator {

// Represents one of the voices an oscillator generates. This is needed since we support polyphony (multiple notes playing simultaneously).
class Voice {
public:
    Voice(Audio::Core::Note note, std::unique_ptr<Audio::Core::Waveform> wf, const Audio::Modulation::Envelope& env);

    // Returns the next sample of this voice
    float GetNextSample();

    // Updates the waveform
    void SetWaveformType(Audio::Core::WaveformInfo::Type type);

    // Updates the octave number
    void SetOctave(int octave);
    
    // Tells the envelope to go into "release" state to fade out the note
    void Release();

    // Returns true if the note is quiet indefinitely from this point and onward, safe to delete
    bool IsDead() const;

    Audio::Core::Note note;
    Audio::Core::Frequency freq;

private:
    std::unique_ptr<Audio::Core::Waveform> m_wf;
    Audio::Modulation::Envelope m_env;
    float m_currentPhase = 0.0f;
};

} // namespace Audio::Generator
