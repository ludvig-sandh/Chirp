// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/core/Waveform.hpp"

#include <cmath>
#include <numbers>

std::unique_ptr<Waveform> Waveform::ConstructWaveform(WaveformInfo::Type type) {
    switch (type) {
        case WaveformInfo::Type::Sine:
            return std::make_unique<Sine>();
        case WaveformInfo::Type::Square:
            return std::make_unique<Square>();
        case WaveformInfo::Type::Saw:
            return std::make_unique<Saw>();
        case WaveformInfo::Type::WhiteNoise:
            return std::make_unique<WhiteNoise>();
        case WaveformInfo::Type::Triangle:
            return std::make_unique<Triangle>();
        case WaveformInfo::Type::Organ:
            return std::make_unique<Organ>();
    }
    throw std::invalid_argument("Cannot construct a waveform from unexpected WaveformType enum value");
    return nullptr;
}

float Saw::GetSampleAt(NormalizedFloat phase) noexcept {
    return -1.0f + 2.0f * phase.get();
}

float Sine::GetSampleAt(NormalizedFloat phase) noexcept {
    return std::sin(phase.get() * 2.0f * std::numbers::pi);
}

float Square::GetSampleAt(NormalizedFloat phase) noexcept {
    return phase.get() >= 0.5f ? 1.0f : -1.0f;
}

float WhiteNoise::GetSampleAt(NormalizedFloat phase) noexcept {
    (void)phase;
    
    // Generate a random sample in range [-1.0, 1.0]
    return m_dist(m_gen);
}

float Triangle::GetSampleAt(NormalizedFloat phase) noexcept {
    return -4.0f * std::fabs(phase.get() - 0.5f) + 1.0f;
}

float Organ::GetSampleAt(NormalizedFloat phase) noexcept {
    float fundamental = std::sin(phase.get() * 2.0 * std::numbers::pi);
    float third = 0.4f * std::sin(phase.get() * 2.0 * std::numbers::pi * 3.0f);
    float sixth = 0.2f * std::sin(phase.get() * 2.0 * std::numbers::pi * 6.0f);
    static const float scale = 0.9; // To keep signal in range [-1, 1]
    return scale * (fundamental + third + sixth);
}