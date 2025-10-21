#pragma once

#include "AudioProcessor.hpp"
#include "BiquadFilter.hpp"

#include <memory>

// LP IIR filter using biquad transfer function
class BaseFilter : public AudioProcessor {
public:
    BaseFilter(float cutoff, float Q);

    void ProcessFrame(AudioBufferFrame& output) override;

    void SetCutoff(float cutoff);

    void SetPeaking(float Q);

    // If you want to set both, this call will only compute the coefficients once
    void SetCutoffAndPeaking(float cutoff, float Q);

    void ClearModulations() override;
    void AddCutoffModulation(float deltaCutoff);

protected:
    // Is called every time cutoff or Q is modified, otherwise the changes won't take effect.
    virtual void ComputeAndApplyCoefficients() = 0;

    // Cutoff frequency
    float m_cutoff;
    float m_cutoffModulation = 0.0f;
    float m_Q;

    BiquadFilter m_leftFilter;
    BiquadFilter m_rightFilter;
};