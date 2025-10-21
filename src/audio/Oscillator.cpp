#include "Oscillator.hpp"
#include "AudioEngine.hpp"

Voice::Voice(std::unique_ptr<Waveform> wf, Frequency freq)
    : m_wf(std::move(wf))
    , freq(freq)
{}

float Voice::GetNextSample() {
    float dt = 1.0 / SAMPLE_RATE;
    float dOffset = dt * freq.GetAbsolute();
    m_currentPhase += dOffset;

    // Loop back to always be in range [0, 1]
    m_currentPhase -= static_cast<int>(m_currentPhase);
    
    return m_wf->GetSampleAt(m_currentPhase);
}

void Oscillator::NoteOn(Frequency freq) {
    Voice v(m_factory(), freq);
    m_voices.push_back(std::move(v));
}

void Oscillator::SetWaveformFactory(WaveformFactoryFn factory) {
    m_factory = std::move(factory);
}

void Oscillator::AddPitchModulation(float semitones) {
    for (Voice& v : m_voices) {
        v.freq.AddPitchModulation(semitones);
    }
}

void Oscillator::ClearModulations() {
    for (Voice& v : m_voices) {
        v.freq.ClearModulations();
    }
}

float Oscillator::GetNextSample() {
    float sample = 0.0f;
    for (Voice& v : m_voices) {
        sample += v.GetNextSample();
    }
    return sample;
}