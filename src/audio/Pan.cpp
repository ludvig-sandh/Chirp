// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "Pan.hpp"

#include <algorithm>

void Pan::Set(float pan) {
    pan = std::clamp(pan, 0.0f, 1.0f);
    float rightGainVal = std::min(pan, 0.5f) * 2.0;
    float leftGainVal = std::min(1.0f - pan, 0.5f) * 2.0;
    m_rightGain.SetLinear(rightGainVal);
    m_leftGain.SetLinear(leftGainVal);
}

AudioFrame Pan::Apply(const AudioFrame& frame) {
    return { m_leftGain.Apply(frame.left), m_rightGain.Apply(frame.right) };
}