#include <algorithm>
#include <cmath>

#include "Frequency.hpp"
#include "AudioEngine.hpp"

Frequency::Frequency(float hertz) : hertz(hertz), pitch(0.0) {}

void Frequency::SetPitch(float pitch) {
    // The maximum pitch we should be able to hear is half the sample rate (Nyquist theorem)
    // this->pitch = std::min(pitch, static_cast<float>(SAMPLE_RATE / 2.0));
    this->pitch = pitch;
}

float Frequency::GetAbsolute() {
    // The maximum pitch we should support is half the sample rate (Nyquist theorem)
    // Otherwise we'd get foldback aliasing.
    float actual = hertz * std::pow(2.0, pitch / notesPerOctave);
    return std::min(actual, static_cast<float>(SAMPLE_RATE / 2.0));
}