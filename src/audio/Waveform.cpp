#include "Waveform.hpp"

#include <random>
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
    }
    throw std::invalid_argument("Cannot construct a waveform from unexpected WaveformType enum value");
    return nullptr;
}

float Saw::GetSampleAt(float currentOffset) {
    return -1.0f + 2.0f * currentOffset;
}

float Sine::GetSampleAt(float currentOffset) {
    return std::sin(currentOffset * 2.0 * std::numbers::pi);
}

float Square::GetSampleAt(float currentOffset) {
    return currentOffset >= 0.5f ? 1.0f : -1.0f;
}

float WhiteNoise::GetSampleAt(float currentOffset) {
    // Generate a random sample in range [-1.0, 1.0]
    return m_dist(m_gen);
}