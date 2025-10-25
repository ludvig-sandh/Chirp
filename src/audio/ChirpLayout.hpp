#pragma once

#include "AudioLayout.hpp"
#include "AudioProcessor.hpp"
#include "AudioPreset.hpp"
#include "Oscillator.hpp"
#include "LowPassFilter.hpp"
#include "HighPassFilter.hpp"
#include "FeedbackDelay.hpp"
#include "RandomLFO.hpp"
#include "PeriodicLFO.hpp"
#include "Reverb.hpp"
#include "Mixer.hpp"
#include "ModulationMatrix.hpp"
#include <memory>

// Defines the audio functionality of the entire chirp tab
class ChirpLayout final: public AudioLayout {
public:
    ChirpLayout();
    std::shared_ptr<AudioProcessor> GetRootNode() override;
    void LoadPreset(AudioPreset& preset) override;
    void ApplyAllModulations() override;

private:
    // Audio nodes
    std::shared_ptr<Oscillator> m_sineOsc;
    std::shared_ptr<Oscillator> m_noiseOsc;
    std::shared_ptr<LowPassFilter> m_lpFilter;
    std::shared_ptr<HighPassFilter> m_hpFilter;
    std::shared_ptr<Reverb> m_reverb;
    std::shared_ptr<Mixer> m_mixer;

    // LFOs
    std::shared_ptr<RandomLFO> m_rnd;
    std::shared_ptr<PeriodicLFO> m_vibrato;

    ModulationMatrix m_modMatrix;

    static inline const float SINEOSC_FREQ_HZ = 2220.0f;
    static inline const float RANDOM_LFO_FREQ = 16.0f;
    static inline const float RANDOM_LFO_AMT = 64.0f;
    static inline const float VIBRATO_LFO_FREQ = 62.0f; 
    static inline const float VIBRATO_LFO_AMT = 1.0; 
};