#include "Gain.hpp"

void Gain::Set(float gain) {
    m_targetGain = gain;
}

float Gain::Get() {
    m_currentGain += (m_targetGain - m_currentGain) * s_alpha;
    return m_currentGain;
}