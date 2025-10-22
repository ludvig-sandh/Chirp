// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "Gain.hpp"
#include "AudioFrame.hpp"

class Pan {
public:
    void Set(float pan);
    AudioFrame Apply(const AudioFrame& frame);
private:
    Gain m_rightGain;
    Gain m_leftGain;
};
