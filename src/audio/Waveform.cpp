#include "Waveform.hpp"
#include "AudioEngine.hpp"

#include <cmath>

float Sine::GetSampleAt(float currentOffset) {
    return std::sin(currentOffset * 2.0 * M_PI);
}

float Square::GetSampleAt(float currentOffset) {
    return currentOffset >= 0.5 ? 1.0 : 0.0;
}

float Saw::GetSampleAt(float currentOffset) {
    return currentOffset;
}