// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/engine/AudioProcessorNode.hpp"
#include "audio/core/Gain.hpp"

// Represents an AudioProcessorNode base class that specifically generates sound, in contrast to one that modifies sound (Effects)
class Generator : public AudioProcessorNode {
public:
    Generator() noexcept;
    virtual ~Generator() noexcept = default;

    void ProcessFrame(AudioFrame& output) override;

    // Computes the next sample in the signal to generate
    virtual float GetNextSample() = 0;

private:
    Gain m_headroom;
    static constexpr float HEADROOM_LEVEL_DB = -12.0f; // Generate signal with some headroom, not at max volume.
};