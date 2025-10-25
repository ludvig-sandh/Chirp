// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <vector>
#include <cmath>

// A DSP block that implements a delay (not to confuse with feedback delay).
// It simply shifts the signal in time (no feeding back to itself).
// Used for example to shift one channel in the ping-pong feedback delay effect.
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
