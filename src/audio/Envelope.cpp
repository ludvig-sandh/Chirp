#include "Envelope.hpp"
#include "AudioEngine.hpp"

Envelope::Envelope(float attack, float hold, float dec, float sus) : attack(attack), hold(hold), dec(dec), sus(sus) {
    assert(sus >= 0.0 && sus <= 1.0 && "");
}

float Envelope::GetNextSample() {
    // Progress time
    m_timeSinceStart += 1.0f / SAMPLE_RATE;

    if (m_timeSinceStart < attack) {
        return m_timeSinceStart / attack;
    }
    if (m_timeSinceStart < attack + hold) {
        return 1.0;
    }
    if (m_timeSinceStart < attack + hold + dec) {
        float timeSinceDecStart = m_timeSinceStart - attack - hold;
        return 1.0 - (1.0 - sus) * timeSinceDecStart / dec;
    }
    return sus;
}

void Envelope::Restart() {
    m_timeSinceStart = 0.0f;
}