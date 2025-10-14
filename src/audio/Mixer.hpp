#pragma once

#include "AudioProcessor.hpp"

// Simple effect that's entire purpose is to group together other processors and control the combined output gain
class Mixer : public AudioProcessor {
public:
    void ClearModulations() override;
    void ProcessFrame(AudioBufferFrame& frame) override;
    
    float targetGain = 1.0;
private:
    float m_currentGain = 0.0;
};
