// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <vector>
#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/engine/AudioFrame.hpp"

class Reverb : public AudioProcessorNode {
public:
    Reverb();

    void SetParams(float feedback, float damping, float wet) noexcept;

    void ProcessFrame(AudioFrame& output) override;

private:
    std::vector<int> combDelays{1116, 1188, 1277, 1356}; // Choose reasonable delay lengths (prime numbers help)
    std::vector<int> allpassDelays{225, 556};
    std::vector<AudioFrame> combFilterState{4};

    std::vector<std::vector<AudioFrame>> combBuffers;
    std::vector<std::vector<AudioFrame>> allpassBuffers;
    std::vector<int> posComb = {0, 0, 0, 0};
    std::vector<int> posAllpass = {0, 0};

    float feedback = 0.8f; // 0.5–0.9 typical
    float damp = 0.2f; // 0–1 lowpass factor inside combs
    float wetMix = 0.3f; // 0–1 dry/wet
};