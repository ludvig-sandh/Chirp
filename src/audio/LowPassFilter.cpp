#include "LowPassFilter.hpp"
#include "AudioEngine.hpp"

#include <math.h>

LowPassFilter::LowPassFilter(float cutoff, unsigned int steepness)
    : m_lastOutput({0.0f, 0.0f})
{
    SetCutoff(cutoff);
    SetSteepness(steepness);
}

void LowPassFilter::ProcessFrame(AudioBufferFrame& output) {
    float left = output.left;
    float right = output.right;
    for (size_t i = 0; i < m_numStages; ++i) {
        m_stages[0][i] += m_alpha * (left - m_stages[0][i]);
        left = m_stages[0][i];

        m_stages[1][i] += m_alpha * (right - m_stages[1][i]);
        right = m_stages[1][i];
    }

    output.left = left;
    output.right = right;
}

void LowPassFilter::SetCutoff(float cutoff) {
    m_cutoff = cutoff;

    float dt = 1.0f / SAMPLE_RATE;
    float RC = 1.0f / (2.0f * M_PI * cutoff);
    m_alpha = dt / (RC + dt);
}

void LowPassFilter::SetSteepness(unsigned int steepness) {
    if (steepness < 0 || steepness > 4) {
        throw std::invalid_argument("Cannot configure a lowpass filter with another steepness level than 1, 2, 3, or 4.");
    }

    m_numStages = steepness;

    m_stages[0].resize(m_numStages, 0.0f);
    m_stages[1].resize(m_numStages, 0.0f);
}