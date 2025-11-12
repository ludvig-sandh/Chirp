// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/generator/Generator.hpp"

namespace Audio::Generator {

Generator::Generator() noexcept {
    m_headroom.SetDecibels(HEADROOM_LEVEL_DB);
}

void Generator::ProcessFrame(Audio::Engine::AudioFrame& output) {
    float value = m_headroom.Apply(GetNextSample());
    output += value;
}

} // namespace Audio::Generator
