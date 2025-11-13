// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "Voice.hpp"
#include "audio/engine/AudioEngine.hpp"
#include <cmath>

namespace Audio::Generator {

Voice::Voice(Audio::Core::Note note, std::unique_ptr<Audio::Core::Waveform> wf, const Audio::Modulation::Envelope& env)
    : note(note)
    , freq(note)
    , m_wf(std::move(wf))
    , m_env(env)
{}

float Voice::GetNextSample() {
    float dt = 1.0 / Audio::Engine::Constants::SAMPLE_RATE;
    float dOffset = dt * freq.GetAbsolute();
    m_currentPhase += dOffset;

    // Loop back to always be in range [0, 1]
    m_currentPhase -= std::floor(m_currentPhase);

    return m_wf->GetSampleAt(m_currentPhase) * m_env.GetNextSample();
}

void Voice::SetWaveformType(Audio::Core::WaveformInfo::Type type) {
    m_wf = Audio::Core::Waveform::ConstructWaveform(type);
}

void Voice::SetOctave(int octave) {
    freq.SetPitch(octave * Audio::Core::Note::NOTES_PER_OCTAVE);
}

void Voice::Release() {
    m_env.Release();
}

bool Voice::IsDead() const {
    return m_env.IsComplete();
}

} // namespace Audio::Generator
