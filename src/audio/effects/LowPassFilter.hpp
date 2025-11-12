// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/effects/BaseFilter.hpp"
#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/effects/dsp/BiquadFilter.hpp"

namespace Audio::Effects {

// LP IIR filter using biquad transfer function
class LowPassFilter final : public BaseFilter {
public:
    LowPassFilter(Audio::Core::Frequency cutoff = Audio::Core::Frequency(1000.0f), float Q = BaseFilter::DEFAULT_Q) noexcept;

private:
    void ComputeAndApplyCoefficients() noexcept override;
};

} // namespace Audio::Effects