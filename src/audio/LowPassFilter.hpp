#pragma once

#include <array>
#include <vector>
#include "AudioProcessor.hpp"

// LP IIR filter
class LowPassFilter final : public AudioProcessor {
public:
    LowPassFilter(float cutoff, unsigned int steepness);

    void ProcessFrame(AudioBufferFrame& output) override;

    void SetCutoff(float cutoff);

    void SetSteepness(unsigned int steepness);

private:

    // Cutoff frequency
    float m_cutoff;

    size_t m_numStages;

    float m_alpha;

    // One per channel
    std::array<float, 2> m_lastOutput;
    std::array<std::vector<float>, 2> m_stages;
};