// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/core/Frequency.hpp"
#include <random>
#include <memory>

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
    virtual ~Waveform() {}
    // Returns the sample value at a specific phase in the waveform in the range [0, 1]
    virtual float GetSampleAt(float phase) = 0;
    static std::unique_ptr<Waveform> ConstructWaveform(WaveformInfo::Type type);
};

class Saw final : public Waveform {
public:
    float GetSampleAt(float phase) override;
};

class Sine final : public Waveform {
public:
    float GetSampleAt(float phase) override;
};

class Square final : public Waveform {
public:
    float GetSampleAt(float phase) override;
};

class WhiteNoise final : public Waveform {
public:
    WhiteNoise() : m_gen(1337), m_dist(-1.0f, 1.0f) {}
    float GetSampleAt(float phase) override;
private:
    std::mt19937 m_gen;
    std::uniform_real_distribution<float> m_dist;
};

class Triangle final : public Waveform {
public:
    float GetSampleAt(float phase) override;
};

class Organ final : public Waveform {
public:
    float GetSampleAt(float phase) override;
};
