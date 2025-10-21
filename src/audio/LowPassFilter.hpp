#pragma once

#include "BaseFilter.hpp"
#include "AudioProcessor.hpp"
#include "BiquadFilter.hpp"

// LP IIR filter using biquad transfer function
class LowPassFilter final : public BaseFilter {
public:
    LowPassFilter(Frequency cutoff, float Q = 0.70710678);

private:
    void ComputeAndApplyCoefficients() override;
};