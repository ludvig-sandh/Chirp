#pragma once

#include "AudioFrame.hpp"

// Keeps track of gain values. When updating gain, it needs to change smoothly rather than jump in value.
// The Gain class takes care of this.
class Gain {
public:
    // Updates the target gain
    void SetLinear(float linear);
    void SetDecibels(float dB);
    
    float GetLinear() const;
    float GetDecibels() const;

    // Applies the current gain and updates it towards the target gain.
    // Should be called once for each sample.
    float Apply(float sample);
    AudioFrame Apply(const AudioFrame& frame);

private:
    float m_currentLinear = 0.0;
    float m_targetLinear = 1.0;

    const float s_maxLinear = 1.0f;
    const float s_minLinear = 0.0f;
    const float s_maxDecibels = 0.0f;
    const float s_minDecibels = -100.0f;

    // The rate at which currentGain should reach targetGain
    // Eg. 0.01 would mean it closes in by 1% each sample.
    static inline const float s_alpha = 0.01;
};