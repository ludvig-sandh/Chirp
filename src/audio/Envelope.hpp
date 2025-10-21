#pragma once

#include <cassert>
#include "LFO.hpp"

class Envelope : public LFO {
public:
    Envelope() : m_attack(0.0), m_hold(0.0), m_dec(0.0), m_sus(1.0) {} // "invisible" envelope, just lets the signal pass through
    Envelope(float attack, float hold, float dec, float sus);

    float GetNextSample() override;

    // Starts the envelope back from the beginning
    void Restart();

private:
    float m_timeSinceStart = 0.0;
    float m_attack;
    float m_hold;
    float m_dec;
    float m_sus;
};