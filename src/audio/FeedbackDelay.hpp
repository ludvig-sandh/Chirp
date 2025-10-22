#pragma once

#include "FeedbackDelayInfo.hpp"
#include "FeedbackDelayLine.hpp"
#include "AudioProcessor.hpp"
#include "Delay.hpp"

// Adds a looping delay effect that repeats audio but with less volume (depending on feedback)
// The processed audio contains both dry input signal and wet delay tail
class FeedbackDelay : public AudioProcessor {
public: 
    FeedbackDelay(FeedbackDelayInfo::Type delayType, float delayTime, float feedback);

    void SetDelayType(FeedbackDelayInfo::Type delayType) noexcept;
    void SetDelayTime(float delayTime) noexcept;
    void SetFeedback(float feedback) noexcept;

    void ProcessFrame(AudioFrame& output) override;

private:
    void UpdateDelayLines() noexcept;

    FeedbackDelayInfo::Type m_delayType;
    float m_delayTime;
    float m_feedback;

    FeedbackDelayLine m_leftLine;
    FeedbackDelayLine m_rightLine;
    FeedbackDelayLine m_monoLine;

    // Will be utilized for ping-pong effect, where one channel gets offset in time
    // to alternate between left/right delays.
    Delay m_preDelay;
};