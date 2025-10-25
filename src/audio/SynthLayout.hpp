#pragma once

#include "AudioLayout.hpp"
#include "AudioProcessor.hpp"
#include "AudioPreset.hpp"
#include "Oscillator.hpp"
#include "LowPassFilter.hpp"
#include "HighPassFilter.hpp"
#include "FeedbackDelay.hpp"
#include "Reverb.hpp"
#include "Mixer.hpp"
#include "Envelope.hpp"
#include "ModulationMatrix.hpp"
#include "LFO.hpp"
#include "PeriodicLFO.hpp"
#include "RandomLFO.hpp"
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
    void AddModulationRoutesForLfoConfig(const LFOConfig& config, AudioPreset& preset);

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