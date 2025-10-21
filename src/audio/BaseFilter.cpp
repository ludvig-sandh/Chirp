#include "BaseFilter.hpp"
#include "AudioEngine.hpp"
#include "LowPassFilter.hpp"
#include "HighPassFilter.hpp"

#include <algorithm>

BaseFilter::BaseFilter(float cutoff, float Q) : m_cutoff(cutoff), m_Q(Q) {}

void BaseFilter::ProcessFrame(AudioBufferFrame& output) {
    output.left = m_leftFilter.Step(output.left);
    output.right = m_rightFilter.Step(output.right);
}

void BaseFilter::SetCutoff(float cutoff) {
    m_cutoff = cutoff;
    m_cutoff = std::clamp(m_cutoff, 20.0f, 20000.0f);
    ComputeAndApplyCoefficients();
}

void BaseFilter::SetPeaking(float Q) {
    m_Q = Q;
    ComputeAndApplyCoefficients();
}

void BaseFilter::SetCutoffAndPeaking(float cutoff, float Q) {
    m_cutoff = cutoff;
    m_cutoff = std::clamp(m_cutoff, 20.0f, 20000.0f);
    m_Q = Q;
    ComputeAndApplyCoefficients();
}

void BaseFilter::ClearModulations() {
    m_cutoffModulation = 0.0f;
}

void BaseFilter::AddCutoffModulation(float deltaCutoff) {
    m_cutoffModulation = std::clamp(m_cutoffModulation + deltaCutoff, 20.0f, 20000.0f);
    ComputeAndApplyCoefficients();
}