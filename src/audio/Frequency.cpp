#include <algorithm>
#include <cmath>

#include "Frequency.hpp"
#include "AudioEngine.hpp"

Frequency::Frequency(float hertz) : m_hertz(hertz), m_pitchBase(0.0), m_pitchModulation(0.0) {}

void Frequency::SetPitch(float pitch) {
    m_pitchBase = pitch;
}

void Frequency::ClearModulations() {
    m_pitchModulation = 0.0;
}

void Frequency::AddPitchModulation(float pitch) {
    m_pitchModulation += pitch;
}

float Frequency::GetAbsolute() {
    // The maximum pitch we should support is half the sample rate (Nyquist theorem)
    // Otherwise we'd get foldback aliasing.
    float actual = m_hertz * std::pow(2.0, (m_pitchBase + m_pitchModulation) / m_notesPerOctave);
    return std::min(actual, static_cast<float>(SAMPLE_RATE / 2.0));
}