// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <vector>
#include "AudioProcessor.hpp"
#include "AudioFrame.hpp"

class Reverb : public AudioProcessor {
public:
    Reverb();

    void SetParams(float roomSize, float damping, float wet);

    void ProcessFrame(AudioFrame& output) override;

private:
    std::vector<int> combDelays;
    std::vector<int> allpassDelays;

    std::vector<std::vector<AudioFrame>> combBuffers;
    std::vector<std::vector<AudioFrame>> allpassBuffers;
    std::vector<AudioFrame> combFilterState;
    std::vector<int> posComb = {0,0,0,0};
    std::vector<int> posAllpass = {0,0};

    float feedback = 0.8f;
    float damp = 0.2f;
    float wetMix = 0.3f;
};