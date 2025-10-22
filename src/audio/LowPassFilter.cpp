#include "LowPassFilter.hpp"
#include "AudioEngine.hpp"
#include <numbers>

LowPassFilter::LowPassFilter(Frequency cutoff, float Q) : BaseFilter(cutoff, Q) {
    ComputeAndApplyCoefficients();
}

void LowPassFilter::ComputeAndApplyCoefficients() {
    float cutoffHz = std::clamp(m_cutoff.GetAbsolute(), s_minCutoff, s_maxCutoff);
    float omega0 = 2.0f * std::numbers::pi * cutoffHz / SAMPLE_RATE;
    float alpha = std::sin(omega0) / (2.0f * m_Q);
    float cosOmega0 = std::cos(omega0);

    // Low pass coeffs
    float b0 = (1.0f - cosOmega0) / 2.0f;
    float b1 = 1.0f - cosOmega0;
    float b2 = (1.0f - cosOmega0) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosOmega0;
    float a2 = 1.0f - alpha;

    // Normalize
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;

    m_leftFilter.SetCoefficients(b0, b1, b2, a1, a2);
    m_rightFilter.SetCoefficients(b0, b1, b2, a1, a2);
}