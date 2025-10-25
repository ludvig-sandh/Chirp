// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "layout/ChirpLayout.hpp"

ChirpLayout::ChirpLayout()
    : m_sineOsc(std::make_shared<Oscillator>(WaveformInfo::Type::Sine))
    , m_noiseOsc(std::make_shared<Oscillator>(WaveformInfo::Type::WhiteNoise))
    , m_lpFilter(std::make_shared<LowPassFilter>())
    , m_hpFilter(std::make_shared<HighPassFilter>())
    , m_reverb(std::make_shared<Reverb>())
    , m_mixer(std::make_shared<Mixer>())
    , m_rnd(std::make_shared<RandomLFO>(Frequency(RANDOM_LFO_FREQ)))
    , m_vibrato(std::make_shared<PeriodicLFO>(WaveformInfo::Type::Saw, Frequency(VIBRATO_LFO_FREQ)))
{
    // Now connect all nodes into a graph
    m_lpFilter->AddChild(m_sineOsc);
    m_lpFilter->AddChild(m_noiseOsc);
    m_hpFilter->AddChild(m_lpFilter);
    m_reverb->AddChild(m_hpFilter);
    m_mixer->AddChild(m_reverb);

    m_sineOsc->NoteOn(Frequency(SINEOSC_FREQ_HZ));
    m_noiseOsc->NoteOn(Frequency(1.0f));
    
    // Connect LFOs
    m_modMatrix.AddRoute(ModulationRoute(m_rnd, m_sineOsc, ModulationType::Pitch, RANDOM_LFO_AMT));
    m_modMatrix.AddRoute(ModulationRoute(m_vibrato, m_sineOsc, ModulationType::Pitch, VIBRATO_LFO_AMT));
}

std::shared_ptr<AudioProcessor> ChirpLayout::GetRootNode() {
    return m_mixer;
}

void ChirpLayout::LoadPreset(AudioPreset& preset) {
    m_sineOsc->isOn = preset.chirpOn.load();
    m_sineOsc->gain.SetLinear(preset.chirpVolume.load());
    m_sineOsc->pan.Set(preset.chirpPan.load());

    m_noiseOsc->isOn = preset.chirpNoiseOn.load();
    m_noiseOsc->gain.SetLinear(preset.chirpNoiseVolume.load());

    m_lpFilter->isOn = preset.chirpLpFilterOn.load();
    m_lpFilter->mix = preset.chirpLpFilterMix.load();
    m_lpFilter->SetCutoffAndPeaking(Frequency(preset.chirpLpFilterCutoff.load()), preset.chirpLpFilterQ.load());

    m_hpFilter->isOn = preset.chirpHpFilterOn.load();
    m_hpFilter->mix = preset.chirpHpFilterMix.load();
    m_hpFilter->SetCutoffAndPeaking(Frequency(preset.chirpHpFilterCutoff.load()), preset.chirpHpFilterQ.load());

    m_reverb->isOn = preset.chirpReverbOn.load();
    m_reverb->SetParams(preset.chirpReverbFeedback.load(), preset.chirpReverbDamp.load(), preset.chirpReverbWet.load());

    m_mixer->gain.SetLinear(preset.chirpMasterVolume.load());
}

void ChirpLayout::ApplyAllModulations() {
    m_modMatrix.ApplyModulations();
}