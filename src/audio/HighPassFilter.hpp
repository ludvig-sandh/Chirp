// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "BaseFilter.hpp"
#include "AudioProcessor.hpp"
#include "BiquadFilter.hpp"

// HP IIR filter using biquad transfer function
class HighPassFilter final : public BaseFilter {
public:
    HighPassFilter(Frequency cutoff = Frequency(1000.0f), float Q = 0.70710678);
    
private:
    void ComputeAndApplyCoefficients() override;
};