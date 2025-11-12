// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/effects/BaseFilter.hpp"
#include "audio/engine/AudioEngine.hpp"
#include "audio/effects/LowPassFilter.hpp"
#include "audio/effects/HighPassFilter.hpp"

#include <algorithm>

BaseFilter::BaseFilter(Frequency cutoff, float Q) noexcept : m_cutoff(cutoff), m_Q(Q) {}

void BaseFilter::ProcessFrame(AudioFrame& output) noexcept {
    output.left = m_leftFilter.Step(output.left);
    output.right = m_rightFilter.Step(output.right);
}

void BaseFilter::SetCutoff(Frequency cutoff) noexcept {
    m_cutoff.SetFrequency(std::clamp(cutoff.GetAbsolute(), MIN_CUTOFF, MAX_CUTOFF));
    ComputeAndApplyCoefficients();
}

void BaseFilter::SetPeaking(float Q) noexcept {
    m_Q = std::clamp(Q, MIN_Q, MAX_Q);
    ComputeAndApplyCoefficients();
}

void BaseFilter::SetCutoffAndPeaking(Frequency cutoff, float Q) noexcept {
    m_cutoff.SetFrequency(std::clamp(cutoff.GetAbsolute(), MIN_CUTOFF, MAX_CUTOFF));
    m_Q = std::clamp(Q, MIN_Q, MAX_Q);
    ComputeAndApplyCoefficients();
}

void BaseFilter::ClearModulationsImpl() noexcept {
    m_cutoff.ClearModulations();
    m_modulationQ = 0.0f;
}

void BaseFilter::ApplyModulation(float amount, Modulation::Type modType) noexcept {
    if (modType == Modulation::Type::Cutoff) {
        m_cutoff.AddPitchModulation(amount);
    }else if (modType == Modulation::Type::Peaking) {
        m_modulationQ += amount;
    }
    ComputeAndApplyCoefficients();
}