// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/generator/Generator.hpp"

Generator::Generator() noexcept {
    m_headroom.SetDecibels(HEADROOM_LEVEL_DB);
}

void Generator::ProcessFrame(AudioFrame& output) {
    float value = m_headroom.Apply(GetNextSample());
    output += value;
}