#include "FeedbackDelay.hpp"

FeedbackDelay::FeedbackDelay(FeedbackDelayType delayType, float delayTime, float feedback)
    : m_leftLine(delayTime, feedback)
    , m_rightLine(delayTime, feedback)
    , m_monoLine(delayTime, feedback)
{
    SetDelayType(delayType);
    SetDelayTime(delayTime);
    SetFeedback(feedback);
}

void FeedbackDelay::SetDelayType(FeedbackDelayType delayType) noexcept {
    m_delayType = delayType;
    SetDelayTime(m_delayTime); // Update delay time in case of switch to/from ping pong
}

void FeedbackDelay::SetDelayTime(float delayTime) noexcept {
    m_delayTime = delayTime;
    if (m_delayType == FeedbackDelayType::PingPong) {
        // Since we want to offset one channel by half the delay time,
        // the delay time effectively halves. Doubling it makes the most
        // sense for the user.
        m_leftLine.SetDelayTime(delayTime * 2.0f);
        m_rightLine.SetDelayTime(delayTime * 2.0f);
        m_preDelay.SetDelay(delayTime); // Offset
    }else {
        m_leftLine.SetDelayTime(delayTime);
        m_rightLine.SetDelayTime(delayTime);
        m_monoLine.SetDelayTime(delayTime);
    }
}

void FeedbackDelay::SetFeedback(float feedback) noexcept {
    m_feedback = feedback;
    m_leftLine.SetFeedback(feedback);
    m_rightLine.SetFeedback(feedback);
    m_monoLine.SetFeedback(feedback);
}

void FeedbackDelay::ProcessFrame(AudioFrame& output) {
    float dryL = output.left;
    float dryR = output.right;
    float avg = (dryL + dryR) / 2.0f;
    
    switch (m_delayType) {
        case FeedbackDelayType::Mono: {
            // Feed average of input into mono delay line
            float out = m_monoLine.Process(avg);
            output += AudioFrame{ out, out };
            break;
        }
        case FeedbackDelayType::Stereo: {
            float wetL = m_leftLine.Process(dryL);
            float wetR = m_rightLine.Process(dryR);
            output += AudioFrame{ wetL, wetR };
            break;
        }
        case FeedbackDelayType::PingPong: {
            float delayedL = m_preDelay.Process(avg); // Offset the left channel
            float wetL = delayedL + m_leftLine.Process(delayedL);
            float wetR = m_rightLine.Process(avg);
            output = AudioFrame{ dryL + wetL, dryR + wetR };
            break;
        }
    }
}