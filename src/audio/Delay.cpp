#include "Delay.hpp"

Delay::Delay(DelayType delayType, float delayTime, float feedback)
    : m_delayType(delayType)
    , m_feedback(feedback)
    , m_leftLine(delayTime, feedback)
    , m_rightLine(delayTime, feedback)
    , m_monoLine(delayTime, feedback)
{}

void Delay::SetDelayTime(float delayTime) noexcept {
    m_leftLine.SetDelayTime(delayTime);
    m_rightLine.SetDelayTime(delayTime);
    m_monoLine.SetDelayTime(delayTime);
}

void Delay::SetFeedback(float feedback) noexcept {
    m_feedback = feedback;
    m_leftLine.SetFeedback(feedback);
    m_rightLine.SetFeedback(feedback);
    m_monoLine.SetFeedback(feedback);
}

void Delay::ProcessFrame(AudioFrame& output) {
    float dryL = output.left;
    float dryR = output.right;

    switch (m_delayType) {
        case DelayType::Mono: {
            // Feed average of input into mono delay line
            float avg = (dryL + dryR) / 2.0f;
            float out = m_monoLine.Process(avg);
            output += out;
            break;
        }
        case DelayType::Stereo: {
            float delayedL = m_leftLine.Process(dryL);
            float delayedR = m_rightLine.Process(dryR);
            output += AudioFrame{ delayedL, delayedR };
            break;
        }
        case DelayType::PingPong: {
            // Cross-feedback version
            // float tempL = m_leftLine.Process((dryL + dryR) / 2.0f + delayedR * m_feedback);
            // float tempR = m_rightLine.Process(delayedL * m_feedback);
            // delayedL = tempL;
            // delayedR = tempR;
            // output.left = delayedL;
            // output.right = delayedR;
            break;
        }
    }
}