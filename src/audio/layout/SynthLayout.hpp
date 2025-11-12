// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/layout/AudioLayout.hpp"
#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/preset/AudioPreset.hpp"
#include "audio/generator/Oscillator.hpp"
#include "audio/effects/LowPassFilter.hpp"
#include "audio/effects/HighPassFilter.hpp"
#include "audio/effects/FeedbackDelay.hpp"
#include "audio/effects/Reverb.hpp"
#include "audio/effects/Mixer.hpp"
#include "audio/modulation/Envelope.hpp"
#include "audio/modulation/ModulationMatrix.hpp"
#include "audio/modulation/LFO.hpp"
#include "audio/modulation/PeriodicLFO.hpp"
#include "audio/modulation/RandomLFO.hpp"
#include <memory>
#include <set>

namespace Audio::Layout {

class SynthLayout final: public AudioLayout {
public:
    SynthLayout();
    std::shared_ptr<Audio::Engine::AudioProcessorNode> GetRootNode() override;
    void LoadPreset(Audio::Preset::AudioPreset& preset) override;
    void ApplyAllModulations() override;

private:
    Audio::Modulation::LFOConfig ReadLFO1Config(Audio::Preset::AudioPreset& preset) const;
    Audio::Modulation::LFOConfig ReadLFO2Config(Audio::Preset::AudioPreset& preset) const;
    std::shared_ptr<Audio::Modulation::LFO> GetLFOSourceHelper(Audio::Modulation::LFOConfig::Mode, int lfoNum);
    void AddModulationRoutesForLfoConfig(const Audio::Modulation::LFOConfig& config);

    // Audio nodes
    std::shared_ptr<Audio::Generator::Oscillator> m_oscA;
    std::shared_ptr<Audio::Generator::Oscillator> m_oscB;
    std::shared_ptr<Audio::Effects::LowPassFilter> m_lpFilter;
    std::shared_ptr<Audio::Effects::HighPassFilter> m_hpFilter;
    std::shared_ptr<Audio::Effects::FeedbackDelay> m_delay;
    std::shared_ptr<Audio::Effects::Reverb> m_reverb;
    std::shared_ptr<Audio::Effects::Mixer> m_mixer;

    // LFOs
    std::shared_ptr<Audio::Modulation::PeriodicLFO> m_lfo1Periodic;
    std::shared_ptr<Audio::Modulation::Envelope> m_lfo1Env;
    std::shared_ptr<Audio::Modulation::RandomLFO> m_lfo1Rnd;

    std::shared_ptr<Audio::Modulation::PeriodicLFO> m_lfo2Periodic;
    std::shared_ptr<Audio::Modulation::Envelope> m_lfo2Env;
    std::shared_ptr<Audio::Modulation::RandomLFO> m_lfo2Rnd;

    std::shared_ptr<Audio::Modulation::Envelope> m_filterEnv;

    Modulation::Matrix m_modMatrix;

    std::set<Audio::Core::Note> m_pressedNotes;
};

} // namespace Audio::Layout
