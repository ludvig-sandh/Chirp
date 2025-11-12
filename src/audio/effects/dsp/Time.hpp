// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <chrono>

namespace DSP {

using Seconds = std::chrono::duration<float>;
using Samples = int;

// Convenience functions
constexpr inline Samples ToSamples(Seconds s, float sampleRate) noexcept {
    return static_cast<Samples>(s.count() * sampleRate);
}

constexpr inline Seconds ToSeconds(Samples samples, float sampleRate) noexcept {
    return Seconds(static_cast<float>(samples) / sampleRate);
}

} // namespace DSP