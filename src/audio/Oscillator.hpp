#pragma once

#include "Frequency.hpp"
#include "Generator.hpp"
#include "Waveform.hpp"
#include "LFO.hpp"

class Oscillator final : public Generator, public LFO {
public:
    Oscillator(std::shared_ptr<Waveform>& wf, const Frequency& freq) : frequency(freq), m_waveform(wf) {}

    // Clears all modulations accumulated from LFOs in the last frame so they can modulate the next one
    virtual void ClearModulations() override;

    float GetNextSample() override;
    
    Frequency frequency;

private:
    std::shared_ptr<Waveform> m_waveform;
    float m_currentOffset = 0.0;
    float m_timeSinceStart = 0.0;
};