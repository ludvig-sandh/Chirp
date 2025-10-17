#pragma once

#include "BaseFilter.hpp"
#include "AudioProcessor.hpp"
#include "BiquadFilter.hpp"

// HP IIR filter using biquad transfer function
class HighPassFilter final : public BaseFilter {
public:
    HighPassFilter(float cutoff, float Q = 0.70710678);
    
private:
    void ComputeAndApplyCoefficients() override;
};