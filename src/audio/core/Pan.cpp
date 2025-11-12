// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/core/Pan.hpp"
#include <algorithm>

namespace Audio::Core {

void Pan::Set(Util::NormalizedFloat pan) noexcept {
    m_pan = pan;
}

void Pan::AddModulation(Util::NormalizedFloat modulation) noexcept {
    m_modulation += modulation;
}

void Pan::ClearModulations() noexcept {
    m_modulation = 0.0f;
}

Audio::Engine::AudioFrame Pan::Apply(const Audio::Engine::AudioFrame& frame) noexcept {
    Util::NormalizedFloat pan = m_pan + m_modulation;
    float rightGainVal = std::min(pan.get(), 0.5f) * 2.0;
    float leftGainVal = std::min(1.0f - pan.get(), 0.5f) * 2.0;
    m_rightGain.SetLinear(rightGainVal);
    m_leftGain.SetLinear(leftGainVal);

    return { m_leftGain.Apply(frame.left), m_rightGain.Apply(frame.right) };
}

} // namespace Audio::Core
