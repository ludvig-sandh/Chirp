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

class SynthLayout final: public AudioLayout {
public:
    SynthLayout();
    std::shared_ptr<AudioProcessorNode> GetRootNode() override;
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

    std::set<Note> m_pressedNotes;
};