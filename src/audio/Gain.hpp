#pragma once

// Keeps track of gain values. When updating gain, it needs to change smoothly rather than jump in value.
// The Gain class takes care of this.
class Gain {
public:
    // Updates the target gain
    void Set(float gain);

    // Returns the current gain and updates it towards the target gain.
    // Should be called once for each sample
    float Get();

private:
    float m_currentGain = 0.0;
    float m_targetGain = 1.0;

    // The rate at which currentGain should reach targetGain
    // Eg. 0.01 would mean it closes in by 1% each sample.
    static inline const float s_alpha = 0.01;
};