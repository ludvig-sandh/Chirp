// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/preset/AudioPreset.hpp"
#include <memory>

namespace Audio::Layout {

// Represents an audio processing graph along with a modulation matrix.
// The audio layout specifies the oscillators, effect chains, audio+modulation
// routing.
class AudioLayout {
public:
    virtual std::shared_ptr<Audio::Engine::AudioProcessorNode> GetRootNode() = 0;
    virtual void LoadPreset(Audio::Preset::AudioPreset& preset) = 0;
    void ClearAllModulations() {
        auto root = GetRootNode();
        if (root) {
            root->ClearModulations();
        }
    }
    virtual void ApplyAllModulations() {}
};

} // namespace Audio::Layout
