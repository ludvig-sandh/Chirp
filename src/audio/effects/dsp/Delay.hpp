// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/effects/dsp/Time.hpp"
#include <vector>
#include <cmath>

namespace Audio::Effects::DSP {

// A DSP block that implements a delay (not to confuse with feedback delay).
// It simply shifts the signal in time, so no feeding back to itself.
// Used for example to shift one channel in the ping-pong feedback delay effect.
class Delay {
public:
    Delay(DSP::Seconds delayTime = DSP::Seconds(0.0f));

    // Set delay in seconds
    void SetDelay(DSP::Seconds delayTime);

    // Process one sample at a time
    float Process(float input) noexcept;

private:
    DSP::Seconds m_delayTime;
    std::vector<float> m_buffer;
    int m_writeIndex = 0;
};

} // namespace Audio::Effects::DSP
