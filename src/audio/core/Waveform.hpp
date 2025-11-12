// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/core/Frequency.hpp"
#include "util/NormalizedFloat.hpp"
#include <random>
#include <memory>

namespace Audio::Core {

namespace WaveformInfo {
    enum class Type {
        Saw = 0,
        Sine,
        Square,
        WhiteNoise,
        Triangle,
        Organ
    };

    inline constexpr const char* Names[] = { "Saw", "Sine", "Square", "White noise", "Triangle", "Organ" };
}

// Base class for representing waveforms, such as sine waves or more complex waves  
class Waveform {
public:
    virtual ~Waveform() noexcept = default;

    // Returns the sample value at a specific phase in the waveform in the range [-1.0f, 0.0f]
    virtual float GetSampleAt(Util::NormalizedFloat phase) noexcept = 0;

    // Factory method for creating one of the waveforms
    static std::unique_ptr<Waveform> ConstructWaveform(WaveformInfo::Type type);
};

// Sharp sound
class Saw final : public Waveform {
public:
    float GetSampleAt(Util::NormalizedFloat phase) noexcept override;
};

// Softest possible sound
class Sine final : public Waveform {
public:
    float GetSampleAt(Util::NormalizedFloat phase) noexcept override;
};

// Not as sharp as a saw, but still a lot of overtones
class Square final : public Waveform {
public:
    float GetSampleAt(Util::NormalizedFloat phase) noexcept override;
};

// Just random values
class WhiteNoise final : public Waveform {
public:
    WhiteNoise() : m_gen(1337), m_dist(-1.0f, 1.0f) {}
    float GetSampleAt(Util::NormalizedFloat phase) noexcept override;
private:
    std::mt19937 m_gen;
    std::uniform_real_distribution<float> m_dist;
};

// A mix between saw and sine
class Triangle final : public Waveform {
public:
    float GetSampleAt(Util::NormalizedFloat phase) noexcept override;
};

// Three sines stacked to recreate an organ sound
class Organ final : public Waveform {
public:
    float GetSampleAt(Util::NormalizedFloat phase) noexcept override;
};

} // namespace Audio::Core
