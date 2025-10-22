// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <vector>
#include <cmath>

class Delay {
public:
    Delay(float delaySeconds = 0.0f);

    // Set delay in seconds
    void SetDelay(float delaySeconds);

    // Process one sample at a time
    float Process(float input);

private:
    float m_delayInSeconds;
    std::vector<float> m_buffer;
    int m_writeIndex = 0;
};
