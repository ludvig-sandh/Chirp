// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "layout/AudioLayout.hpp"
#include "engine/AudioProcessor.hpp"
#include "preset/AudioPreset.hpp"
#include "generator/Oscillator.hpp"
#include "effects/LowPassFilter.hpp"
#include "effects/HighPassFilter.hpp"
#include "effects/FeedbackDelay.hpp"
#include "effects/Reverb.hpp"
#include "effects/Mixer.hpp"
#include "modulation/Envelope.hpp"
#include "modulation/ModulationMatrix.hpp"
#include "modulation/LFO.hpp"
#include "modulation/PeriodicLFO.hpp"
#include "modulation/RandomLFO.hpp"
#include <memory>

class SynthLayout final: public AudioLayout {
public:
    SynthLayout();
    std::shared_ptr<AudioProcessor> GetRootNode() override;
    void LoadPreset(AudioPreset& preset) override;
    void ApplyAllModulations() override;

private:
    LFOConfig ReadLFO1Config(AudioPreset& preset) const;
    LFOConfig ReadLFO2Config(AudioPreset& preset) const;
    std::shared_ptr<LFO> GetLFOSourceHelper(LFOConfig::Mode, int lfoNum);
    void AddModulationRoutesForLfoConfig(const LFOConfig& config);

    // Audio nodes
    std::shared_ptr<Oscillator> m_oscA;
    std::shared_ptr<Oscillator> m_oscB;
    std::shared_ptr<LowPassFilter> m_lpFilter;
    std::shared_ptr<HighPassFilter> m_hpFilter;
    std::shared_ptr<FeedbackDelay> m_delay;
    std::shared_ptr<Reverb> m_reverb;
    std::shared_ptr<Mixer> m_mixer;

    // LFOs
    std::shared_ptr<PeriodicLFO> m_lfo1Periodic;
    std::shared_ptr<Envelope> m_lfo1Env;
    std::shared_ptr<RandomLFO> m_lfo1Rnd;

    std::shared_ptr<PeriodicLFO> m_lfo2Periodic;
    std::shared_ptr<Envelope> m_lfo2Env;
    std::shared_ptr<RandomLFO> m_lfo2Rnd;

    std::shared_ptr<Envelope> m_filterEnv;

    ModulationMatrix m_modMatrix;
};