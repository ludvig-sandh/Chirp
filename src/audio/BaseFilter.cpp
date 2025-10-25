// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "BaseFilter.hpp"
#include "AudioEngine.hpp"
#include "LowPassFilter.hpp"
#include "HighPassFilter.hpp"

#include <algorithm>

BaseFilter::BaseFilter(Frequency cutoff, float Q) : m_cutoff(cutoff), m_Q(Q) {}

void BaseFilter::ProcessFrame(AudioFrame& output) {
    output.left = m_leftFilter.Step(output.left);
    output.right = m_rightFilter.Step(output.right);
}

void BaseFilter::SetCutoff(Frequency cutoff) {
    m_cutoff.SetFrequency(std::clamp(cutoff.GetAbsolute(), s_minCutoff, s_maxCutoff));
    ComputeAndApplyCoefficients();
}

void BaseFilter::SetPeaking(float Q) {
    m_Q = std::max(Q, MIN_Q);
    ComputeAndApplyCoefficients();
}

void BaseFilter::SetCutoffAndPeaking(Frequency cutoff, float Q) {
    m_cutoff.SetFrequency(std::clamp(cutoff.GetAbsolute(), s_minCutoff, s_maxCutoff));
    m_Q = std::max(Q, MIN_Q);
    ComputeAndApplyCoefficients();
}

void BaseFilter::ClearModulationsImpl() {
    m_cutoff.ClearModulations();
    m_modulationQ = 0.0f;
}

void BaseFilter::ApplyModulation(float amount, ModulationType modType) {
    if (modType == ModulationType::Cutoff) {
        m_cutoff.AddPitchModulation(amount);
    }else if (modType == ModulationType::Peaking) {
        m_modulationQ += amount;
    }
    ComputeAndApplyCoefficients();
}