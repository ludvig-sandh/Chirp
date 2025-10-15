#pragma once

#include "Gain.hpp"

class Pan {
public:
    void Set(float pan);
    float GetRightGain();
    float GetLeftGain();
private:
    Gain m_rightGain;
    Gain m_leftGain;
};
