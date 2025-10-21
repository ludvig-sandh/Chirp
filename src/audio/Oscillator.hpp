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

    Frequency freq;

private:
    std::unique_ptr<Waveform> m_wf;
    Envelope m_env;
    float m_currentPhase = 0.0f;
};

class Oscillator final : public Generator, public LFO {
public:
    explicit Oscillator(WaveformInfo::Type type) : m_waveformType(type) {}

    // Start a new voice
    void NoteOn(Frequency freq);

    // Stop an existing voice
    void NoteOff(Frequency freq);

    // Update the waveform used by this oscillator
    void SetWaveformType(WaveformInfo::Type type);

    // Update the envelope used for note volume
    void SetEnvelope(Envelope envelope);

    // Modulates the pitch of all voices
    void AddPitchModulation(float semitones);

    // Clears all modulations accumulated from LFOs in the last frame so they can modulate the next one
    virtual void ClearModulations() override;

    // Returns the next sample for this oscillator. Must be called once every frame or it will become desynched.
    float GetNextSample() override;
    
private:
    WaveformInfo::Type m_waveformType;
    Envelope m_env;
    std::unordered_map<float, Voice> m_voices;
};