// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "engine/AudioProcessor.hpp"
#include "effects/util/BiquadFilter.hpp"
#include "core/Frequency.hpp"
#include "modulation/ModulationMatrix.hpp"

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

    void ClearModulationsImpl() override;
    void ApplyModulation(float amount, ModulationType modType) override;

protected:
    // Is called every time cutoff or Q is modified, otherwise the changes won't take effect.
    virtual void ComputeAndApplyCoefficients() = 0;

    // Cutoff frequency
    Frequency m_cutoff;
    float m_Q;
    float m_modulationQ = 0.0f;

    BiquadFilter m_leftFilter;
    BiquadFilter m_rightFilter;

    static inline const float s_minCutoff = 5.0f;
    static inline const float s_maxCutoff = 20000.0f;
    static inline const float MIN_Q = 0.1f; // To avoid division-by-zero
    static inline const float MAX_Q = 5.0f;
};