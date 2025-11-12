// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/core/Gain.hpp"
#include "audio/engine/AudioFrame.hpp"
#include "util/NormalizedFloat.hpp"

namespace Audio::Core {

class Pan {
public:
    // Sets the pan value, accepts values between 0.0 and 1.0. 0.0 => only left channel, 1.0 => only right channel
    void Set(Util::NormalizedFloat pan) noexcept;
    void AddModulation(Util::NormalizedFloat modulation) noexcept;
    void ClearModulations() noexcept;
    Audio::Engine::AudioFrame Apply(const Audio::Engine::AudioFrame& frame) noexcept;
private:
    Util::NormalizedFloat m_pan = 0.5f; // Center
    Util::NormalizedFloat m_modulation = 0.0f;
    Gain m_rightGain;
    Gain m_leftGain;
};

} // namespace Audio::Core
