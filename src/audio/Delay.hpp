#pragma once

#include "DelayLine.hpp"
#include "AudioProcessor.hpp"

enum class DelayType {
    Mono,
    Stereo,
    PingPong
};


class Delay : public AudioProcessor {
public: 
    Delay(DelayType delayType, float delayTime, float feedback);

    void SetDelayTime(float delayTime) noexcept;
    void SetFeedback(float feedback) noexcept;

    void ProcessFrame(AudioFrame& output) override;

private:
    DelayType m_delayType;
    float m_feedback;

    DelayLine m_leftLine;
    DelayLine m_rightLine;
    DelayLine m_monoLine;
};