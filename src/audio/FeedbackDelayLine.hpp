#pragma once

#include <vector>
#include <algorithm>

// A single-channel delay line building block for different types of delay effects
class FeedbackDelayLine {
public:
    // Constants for safety
    static constexpr float MIN_DELAY_SEC = 0.0f;
    static constexpr float MAX_DELAY_SEC = 10.0f;
    static constexpr float MAX_FEEDBACK  = 0.99f;

    FeedbackDelayLine(float delayTime, float feedback);

    // Adjust delay time (seconds), safely clamped to allowed range
    void SetDelayTime(float seconds) noexcept;

    // Adjust feedback amount [0.0, 0.99]
    void SetFeedback(float fb) noexcept;

    // Process one input sample and return the delayed output (without the input signal)
    float Process(float input) noexcept;

    // Reset the delay line buffer to silence
    void Clear() noexcept;

private:
    // Initialize internal buffer based on sample rate and max delay
    void Init();

    std::vector<float> m_buffer;
    size_t m_bufferSize   = 0;
    size_t m_writeIndex   = 0;
    size_t m_delaySamples = 0;
    
    float m_feedback   = 0.5f;
};
