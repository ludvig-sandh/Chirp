#pragma once

#include "Gain.hpp"

class Pan {
public:
    void Set(float pan);
    float ApplyRightGain(float sample);
    float ApplyLeftGain(float sample);
private:
    Gain m_rightGain;
    Gain m_leftGain;
};
