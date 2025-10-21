#include "Envelope.hpp"
#include "AudioEngine.hpp"

Envelope::Envelope(float attack, float hold, float dec, float sus) : m_attack(attack), m_hold(hold), m_dec(dec), m_sus(sus) {
    assert(sus >= 0.0 && sus <= 1.0 && "");
}

float Envelope::GetNextSample() {
    // Progress time
    m_timeSinceStart += 1.0f / SAMPLE_RATE;

    if (m_timeSinceStart < m_attack) {
        return m_timeSinceStart / m_attack;
    }
    if (m_timeSinceStart < m_attack + m_hold) {
        return 1.0;
    }
    if (m_timeSinceStart < m_attack + m_hold + m_dec) {
        float timeSinceDecStart = m_timeSinceStart - m_attack - m_hold;
        return 1.0 - (1.0 - m_sus) * timeSinceDecStart / m_dec;
    }
    return m_sus;
}