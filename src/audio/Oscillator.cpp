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

void Voice::SetWaveformType(WaveformInfo::Type type) {
    m_wf = Waveform::ConstructWaveform(type);
}

void Oscillator::NoteOn(Frequency freq) {
    Voice v(Waveform::ConstructWaveform(m_waveformType), freq);
    m_voices.emplace(freq.GetAbsolute(), std::move(v));
}

void Oscillator::NoteOff(Frequency freq) {
    m_voices.erase(freq.GetAbsolute());
}

void Oscillator::SetWaveformType(WaveformInfo::Type type) {
    if (type == m_waveformType) {
        return;
    }
    m_waveformType = type;

    // Update all playing notes to use new waveform instead
    for (auto& [_, voice] : m_voices) {
        voice.SetWaveformType(type);
    }
}

void Oscillator::AddPitchModulation(float semitones) {
    for (auto& [_, voice] : m_voices) {
        voice.freq.AddPitchModulation(semitones);
    }
}

void Oscillator::ClearModulations() {
    for (auto& [_, voice] : m_voices) {
        voice.freq.ClearModulations();
    }
}

float Oscillator::GetNextSample() {
    float sample = 0.0f;
    for (auto& [_, voice] : m_voices) {
        sample += voice.GetNextSample();
    }
    return sample;
}