#include "Waveform.hpp"
#include "AudioEngine.hpp"

#include <random>
#include <cmath>

float Sine::GetSampleAt(float currentOffset) {
    return std::sin(currentOffset * 2.0 * M_PI) / 2.0 + 0.5; // Shift to [0, 1] interval
}

float Square::GetSampleAt(float currentOffset) {
    return currentOffset >= 0.5 ? 1.0 : 0.0;
}

float Saw::GetSampleAt(float currentOffset) {
    return currentOffset;
}

float WhiteNoise::GetSampleAt(float currentOffset) {
    // Generate a random sample in range [0.0, 1.0]
    return m_dist(m_gen);
}