// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/engine/AudioProcessorNode.hpp"

namespace Audio::Effects {

// The Mixer class doesn't provide any extra functionality, but is a great semantically to use
// if the only thing you need is to group together outputs.
class Mixer : public Audio::Engine::AudioProcessorNode {
public:
    // Empty implementation means audio passes through.
    void ProcessFrame(Audio::Engine::AudioFrame&) override {}
};

} // namespace Audio::Effects