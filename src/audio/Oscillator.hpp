#pragma once

#include "Frequency.hpp"
#include "Generator.hpp"
#include "Waveform.hpp"
#include "LFO.hpp"

class Oscillator final : public Generator, public LFO {
public:
    Oscillator(std::shared_ptr<Waveform>& wf, const Frequency& freq) : m_waveform(wf), frequency(freq) {}

    float GetNextSample() override;

private:
    std::shared_ptr<Waveform> m_waveform;
    Frequency frequency;
    float m_currentOffset = 0.0;
    float m_timeSinceStart = 0.0;
};