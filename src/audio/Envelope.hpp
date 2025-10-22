// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <cassert>
#include "LFO.hpp"

class Envelope : public LFO {
public:
    Envelope() : attack(0.0), hold(0.0), dec(0.0), sus(1.0) {} // "invisible" envelope, just lets the signal pass through
    Envelope(float attack, float hold, float dec, float sus);

    float GetNextSample() override;

    // Starts the envelope back from the beginning
    void Restart();

    float attack;
    float hold;
    float dec;
    float sus;

private:
    float m_timeSinceStart = 0.0;
};