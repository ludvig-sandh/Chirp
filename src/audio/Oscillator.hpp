#pragma once

#include "Frequency.hpp"
#include "Generator.hpp"
#include "Waveform.hpp"
#include "LFO.hpp"

#include <unordered_map>

class Voice {
public:
    Voice(std::unique_ptr<Waveform> wf, Frequency freq);
    float GetNextSample();

private:
    std::unique_ptr<Waveform> m_wf;
    float m_currentPhase = 0.0f;

public:
    Frequency freq;
};

class Oscillator final : public Generator, public LFO {
public:
    using WaveformFactoryFn = std::function<std::unique_ptr<Waveform>()>;

    explicit Oscillator(WaveformFactoryFn factory) : m_factory(std::move(factory)) {}

    // Start a new voice
    void NoteOn(Frequency freq);

    // Stop an existing voice
    void NoteOff(Frequency freq);

    // Update the waveform used by this oscillator
    void SetWaveformFactory(WaveformFactoryFn factory);

    // Modulates the pitch of all voices
    void AddPitchModulation(float semitones);

    // Clears all modulations accumulated from LFOs in the last frame so they can modulate the next one
    virtual void ClearModulations() override;

    // Returns the next sample for this oscillator. Must be called once every frame or it will become desynched.
    float GetNextSample() override;
    
private:
    WaveformFactoryFn m_factory;
    std::unordered_map<float, Voice> m_voices;
};