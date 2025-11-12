// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/effects/BaseFilter.hpp"
#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/effects/dsp/BiquadFilter.hpp"

// HP IIR filter using biquad transfer function
class HighPassFilter final : public BaseFilter {
public:
    HighPassFilter(Frequency cutoff = Frequency(1000.0f), float Q = BaseFilter::DEFAULT_Q) noexcept;
    
private:
    void ComputeAndApplyCoefficients() noexcept override;
};