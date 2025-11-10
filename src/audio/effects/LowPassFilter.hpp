// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/effects/BaseFilter.hpp"
#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/effects/util/BiquadFilter.hpp"

// LP IIR filter using biquad transfer function
class LowPassFilter final : public BaseFilter {
public:
    LowPassFilter(Frequency cutoff = Frequency(1000.0f), float Q = 0.70710678);

private:
    void ComputeAndApplyCoefficients() override;
};