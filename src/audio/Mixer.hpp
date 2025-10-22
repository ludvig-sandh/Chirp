// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "AudioProcessor.hpp"

// The Mixer class doesn't provide any extra functionality, but is a great semantically to use
// if the only thing you need is to group together outputs.
class Mixer : public AudioProcessor {
public:
    // Empty implementation means audio passes through.
    void ProcessFrame(AudioFrame& frame) override {}
};
