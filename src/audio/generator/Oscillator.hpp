// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/core/Frequency.hpp"
#include "audio/generator/Generator.hpp"
#include "audio/core/Waveform.hpp"
#include "audio/modulation/Envelope.hpp"
#include "audio/modulation/LFO.hpp"
#include "audio/generator/Voice.hpp"
#include <vector>

namespace Audio::Generator {

class Oscillator final : public Generator {
public:
    explicit Oscillator(Audio::Core::WaveformInfo::Type type = Audio::Core::WaveformInfo::Type::Saw) : m_waveformType(type) {}

    // Start a new voice
    void NoteOn(Audio::Core::Note note);

    // Stop an existing voice
    void NoteOff(Audio::Core::Note note);

    // Update the waveform used by this oscillator
    void SetWaveformType(Audio::Core::WaveformInfo::Type type);

    // Update the envelope used for note volume
    void SetEnvelope(Audio::Modulation::Envelope envelope);

    // Updates the octave offset (for example 1 means every note is played 1 octave higher)
    void SetOctave(int octave);

    // Apply a modulation to this destination
    void ApplyModulation(float amount, Modulation::Type modType) noexcept override;

    // Clears all modulations accumulated from LFOs in the last frame so they can modulate the next one
    virtual void ClearModulationsImpl() noexcept override;

    // Returns the next sample for this oscillator. Must be called once every frame or it will become desynched.
    float GetNextSample() override;
    
private:
    // Remove voices lazily which allows them to play the "release" of a note
    void CleanUpDeadNotes();

    Audio::Core::WaveformInfo::Type m_waveformType;
    Audio::Modulation::Envelope m_env;
    std::vector<Voice> m_voices;
};

} // namespace Audio::Generator
