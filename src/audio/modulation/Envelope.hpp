// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <cassert>
#include "audio/modulation/LFO.hpp"
#include "util/NormalizedFloat.hpp"
#include "audio/effects/dsp/Time.hpp"

namespace Audio::Modulation {

// Models a typical ASDR envelope
class Envelope : public LFO {
public:
    // "invisible" envelope, just lets the signal pass through
    Envelope() noexcept : attack(0.0f), hold(0.0f), decay(0.0f), sustain(1.0f), release(0.0f) {}
    Envelope(
        Audio::Effects::DSP::Seconds atk,
        Audio::Effects::DSP::Seconds hld,
        Audio::Effects::DSP::Seconds dec,
        Util::NormalizedFloat sus,
        Audio::Effects::DSP::Seconds rel
    ) noexcept;

    // Returns the amplitude of the amplitude at the current time and progresses time forward by the sample period
    float GetNextSample() noexcept override;

    // Starts the envelope back from the beginning
    void Restart() noexcept;

    // Envelope goes into release mode (from sustain to quiet) when a note is released
    void Release() noexcept;

    // Returns true if the envelope has gone through all stages and will be quiet until it is restarted.
    bool IsComplete() const noexcept;

    Audio::Effects::DSP::Seconds attack;
    Audio::Effects::DSP::Seconds hold;
    Audio::Effects::DSP::Seconds decay;
    Util::NormalizedFloat sustain;
    Audio::Effects::DSP::Seconds release;
    
private:
    float GetNextSampleHelper() noexcept;

    Audio::Effects::DSP::Seconds m_timeSinceStart{0.0f};
    bool m_hasBeenReleased = false;
    float m_lastValueBeforeRelease = 0.0f;
};

} // namespace Audio::Modulation
