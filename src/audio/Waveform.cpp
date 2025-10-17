#include "Waveform.hpp"
#include "AudioEngine.hpp"

#include <random>
#include <cmath>

float Sine::GetSampleAt(float currentOffset) {
    return std::sin(currentOffset * 2.0 * M_PI);
}

float Square::GetSampleAt(float currentOffset) {
    return currentOffset >= 0.5f ? 1.0f : -1.0f;
}

float Saw::GetSampleAt(float currentOffset) {
    return -1.0f + 2.0f * currentOffset;
}

float WhiteNoise::GetSampleAt(float currentOffset) {
    // Generate a random sample in range [-1.0, 1.0]
    return m_dist(m_gen);
}