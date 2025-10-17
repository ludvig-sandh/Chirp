#include "Pan.hpp"

#include <algorithm>

void Pan::Set(float pan) {
    pan = std::clamp(pan, 0.0f, 1.0f);
    float rightGainVal = std::min(pan, 0.5f) * 2.0;
    float leftGainVal = std::min(1.0f - pan, 0.5f) * 2.0;
    m_rightGain.SetLinear(rightGainVal);
    m_leftGain.SetLinear(leftGainVal);
}

float Pan::ApplyRightGain(float sample) {
    return m_rightGain.Apply(sample);
}

float Pan::ApplyLeftGain(float sample) {
    return m_leftGain.Apply(sample);
}