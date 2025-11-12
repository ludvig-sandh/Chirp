// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/effects/dsp/BiquadFilter.hpp"
#include "audio/core/Frequency.hpp"
#include "audio/modulation/ModulationMatrix.hpp"

#include <memory>

// LP IIR filter using biquad transfer function
class BaseFilter : public AudioProcessorNode {
public:
    BaseFilter(Frequency cutoff, float Q) noexcept;
    virtual ~BaseFilter() noexcept {}

    void ProcessFrame(AudioFrame& output) noexcept override;

    void SetCutoff(Frequency cutoff) noexcept;

    void SetPeaking(float Q) noexcept;

    // If you want to set both, this call will only compute the coefficients once
    void SetCutoffAndPeaking(Frequency cutoff, float Q) noexcept;

    void ClearModulationsImpl() noexcept override;
    void ApplyModulation(float amount, ModulationType modType) noexcept override;

protected:
    // Is called every time cutoff or Q is modified, otherwise the changes won't take effect.
    virtual void ComputeAndApplyCoefficients() noexcept = 0;

    // Cutoff frequency
    Frequency m_cutoff;
    float m_Q;
    float m_modulationQ = 0.0f;

    BiquadFilter m_leftFilter;
    BiquadFilter m_rightFilter;

    static inline const float DEFAULT_Q = std::sqrt(2.0) / 2.0f; // Half sqrt(2) used as default q value
    static constexpr float MIN_CUTOFF = 5.0f;
    static constexpr float MAX_CUTOFF = 20000.0f;
    static constexpr float MIN_Q = 0.1f; // To avoid division-by-zero
    static constexpr float MAX_Q = 5.0f;
};