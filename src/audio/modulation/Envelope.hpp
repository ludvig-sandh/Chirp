// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <cassert>
#include "modulation/LFO.hpp"

// Models a typical ASDR envelope
class Envelope : public LFO {
public:
    // "invisible" envelope, just lets the signal pass through
    Envelope() : attack(0.0f), hold(0.0f), decay(0.0f), sustain(1.0f), release(0.0f) {}
    Envelope(float atk, float hld, float dec, float sus, float rel);

    float GetNextSample() override;

    // Starts the envelope back from the beginning
    void Restart();

    // Envelope goes into release mode (from sustain to quiet) when a note is released
    void Release();

    // Returns true if the envelope has gone through all stages and will be quiet until it is restarted.
    bool IsComplete() const;

    float attack;
    float hold;
    float decay;
    float sustain;
    float release;
    
private:

    float m_timeSinceStart = 0.0;
    bool m_hasBeenReleased = false;
};