#include "Pan.hpp"

#include <algorithm>

void Pan::Set(float pan) {
    pan = std::clamp(pan, 0.0f, 1.0f);
    float rightGainVal = std::min(pan, 0.5f) * 2.0;
    float leftGainVal = std::min(1.0f - pan, 0.5f) * 2.0;
    m_rightGain.Set(rightGainVal);
    m_leftGain.Set(leftGainVal);
}

float Pan::GetRightGain() {
    return m_rightGain.Get();
}

float Pan::GetLeftGain() {
    return m_leftGain.Get();
}