#include "Gain.hpp"

#include <cmath>
#include <algorithm>

void Gain::SetLinear(float linear) {
    m_targetLinear = std::clamp(linear, s_minLinear, s_maxLinear);
}

void Gain::SetDecibels(float dB) {
    float clamped = std::clamp(dB, s_minDecibels, s_maxDecibels);
    float linear = std::pow(10.0f, clamped / 20.0f);
    m_targetLinear = linear;
}

float Gain::GetLinear() const {
    return m_currentLinear;
}

float Gain::GetDecibels() const {
    return 20.0f * std::log10(std::max(m_currentLinear, 1e-5f)); // Avoid log(0)
}

float Gain::Apply(float sample) {
    m_currentLinear += (m_targetLinear - m_currentLinear) * s_alpha;
    return m_currentLinear * sample;
}

AudioFrame Gain::Apply(const AudioFrame& frame) {
    m_currentLinear += (m_targetLinear - m_currentLinear) * s_alpha;
    return m_currentLinear * frame;
}