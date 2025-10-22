#include "Delay.hpp"

Delay::Delay(DelayType delayType, float delayTime, float feedback)
    : m_delayType(delayType)
    , m_leftLine(delayTime, feedback)
    , m_rightLine(delayTime, feedback)
{}

void Delay::SetDelayTime(float delayTime) noexcept {
    m_leftLine.SetDelayTime(delayTime);
    m_rightLine.SetDelayTime(delayTime);
}

void Delay::SetFeedback(float feedback) noexcept {
    m_leftLine.SetFeedback(feedback);
    m_rightLine.SetFeedback(feedback);
}

void Delay::ProcessFrame(AudioFrame& output) {
    output.left = m_leftLine.Process(output.left);
    output.right = m_rightLine.Process(output.right);
}