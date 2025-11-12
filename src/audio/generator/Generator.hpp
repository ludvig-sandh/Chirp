// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/core/Gain.hpp"

namespace Audio::Generator {

// Represents an AudioProcessorNode base class that specifically generates sound, in contrast to one that modifies sound (Effects)
class Generator : public Audio::Engine::AudioProcessorNode {
public:
    Generator() noexcept;
    virtual ~Generator() noexcept = default;

    void ProcessFrame(Audio::Engine::AudioFrame& output) override;

    // Computes the next sample in the signal to generate
    virtual float GetNextSample() = 0;

private:
    Audio::Core::Gain m_headroom;
    static constexpr float HEADROOM_LEVEL_DB = -12.0f; // Generate signal with some headroom, not at max volume.
};

} // namespace Audio::Generator
