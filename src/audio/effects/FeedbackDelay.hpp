// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/effects/dsp/FeedbackDelayInfo.hpp"
#include "audio/effects/dsp/FeedbackDelayLine.hpp"
#include "audio/effects/dsp/Delay.hpp"
#include "audio/effects/dsp/Time.hpp"
#include "audio/engine/AudioProcessorNode.hpp"

namespace Audio::Effects {

// Adds a looping delay effect that repeats audio but with less volume (depending on feedback)
// The processed audio contains both dry input signal and wet delay tail
class FeedbackDelay : public Audio::Engine::AudioProcessorNode {
public:
    FeedbackDelay();
    FeedbackDelay(DSP::FeedbackDelayInfo::Type delayType, DSP::Seconds delayTime, float feedback);

    void SetDelayType(DSP::FeedbackDelayInfo::Type delayType) noexcept;
    void SetDelayTime(DSP::Seconds delayTime) noexcept;
    void SetFeedback(float feedback) noexcept;

    void ProcessFrame(Audio::Engine::AudioFrame& output) noexcept override;

private:
    void UpdateDelayLines() noexcept;

    DSP::FeedbackDelayInfo::Type m_delayType = DSP::FeedbackDelayInfo::Type::Mono;
    DSP::Seconds m_delayTime{0.2f};
    float m_feedback = 0.5;

    DSP::FeedbackDelayLine m_leftLine;
    DSP::FeedbackDelayLine m_rightLine;
    DSP::FeedbackDelayLine m_monoLine;

    // Will be utilized for ping-pong effect, where one channel gets offset in time
    // to alternate between left/right delays.
    DSP::Delay m_preDelay;
};

} // namespace Audio::Effects