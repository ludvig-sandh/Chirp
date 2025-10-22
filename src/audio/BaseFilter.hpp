#pragma once

#include "AudioProcessor.hpp"
#include "BiquadFilter.hpp"
#include "Frequency.hpp"

#include <memory>

// LP IIR filter using biquad transfer function
class BaseFilter : public AudioProcessor {
public:
    BaseFilter(Frequency cutoff, float Q);

    void ProcessFrame(AudioFrame& output) override;

    void SetCutoff(Frequency cutoff);

    void SetPeaking(float Q);

    // If you want to set both, this call will only compute the coefficients once
    void SetCutoffAndPeaking(Frequency cutoff, float Q);

    void ClearModulations() override;
    void AddCutoffModulation(float semitones);

protected:
    // Is called every time cutoff or Q is modified, otherwise the changes won't take effect.
    virtual void ComputeAndApplyCoefficients() = 0;

    // Cutoff frequency
    Frequency m_cutoff;
    float m_Q;

    BiquadFilter m_leftFilter;
    BiquadFilter m_rightFilter;

    static inline const float s_minCutoff = 5.0f;
    static inline const float s_maxCutoff = 20000.0f;
};