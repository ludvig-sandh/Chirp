// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "FeedbackDelay.hpp"

FeedbackDelay::FeedbackDelay() 
    : m_leftLine(m_delayTime, m_feedback)
    , m_rightLine(m_delayTime, m_feedback)
    , m_monoLine(m_delayTime, m_feedback)
{
    UpdateDelayLines();
}

FeedbackDelay::FeedbackDelay(FeedbackDelayInfo::Type delayType, float delayTime, float feedback)
    : m_leftLine(delayTime, feedback)
    , m_rightLine(delayTime, feedback)
    , m_monoLine(delayTime, feedback)
{
    SetDelayType(delayType);
    SetDelayTime(delayTime);
    SetFeedback(feedback);
}

void FeedbackDelay::SetDelayType(FeedbackDelayInfo::Type delayType) noexcept {
    if (m_delayType == delayType) {
        return;
    }
    m_delayType = delayType;
    UpdateDelayLines();
}

void FeedbackDelay::SetDelayTime(float delayTime) noexcept {
    if (m_delayTime == delayTime) {
        return; // Don't bother
    }
    m_delayTime = delayTime;
    UpdateDelayLines();
}

void FeedbackDelay::SetFeedback(float feedback) noexcept {
    if (m_feedback == feedback) {
        return;
    }
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
        case FeedbackDelayInfo::Type::Mono: {
            // Feed average of input into mono delay line
            float out = m_monoLine.Process(avg);
            output += AudioFrame{ out, out };
            break;
        }
        case FeedbackDelayInfo::Type::Stereo: {
            float wetL = m_leftLine.Process(dryL);
            float wetR = m_rightLine.Process(dryR);
            output += AudioFrame{ wetL, wetR };
            break;
        }
        case FeedbackDelayInfo::Type::PingPong: {
            float delayedL = m_preDelay.Process(avg); // Offset the left channel
            float wetL = delayedL + m_leftLine.Process(delayedL);
            float wetR = m_rightLine.Process(avg);
            output = AudioFrame{ dryL + wetL, dryR + wetR };
            break;
        }
    }
}

void FeedbackDelay::UpdateDelayLines() noexcept {
    if (m_delayType == FeedbackDelayInfo::Type::PingPong) {
        // Since we want to offset one channel by half the delay time,
        // the delay time effectively halves. Doubling it makes the most
        // sense for the user.
        m_leftLine.SetDelayTime(m_delayTime * 2.0f);
        m_rightLine.SetDelayTime(m_delayTime * 2.0f);
        m_preDelay.SetDelay(m_delayTime); // Offset
    }else {
        m_leftLine.SetDelayTime(m_delayTime);
        m_rightLine.SetDelayTime(m_delayTime);
        m_monoLine.SetDelayTime(m_delayTime);
    }
}