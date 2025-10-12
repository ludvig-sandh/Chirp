#pragma once

#include <algorithm>
#include <cmath>

class Frequency {
public:
    Frequency(float hertz);

    void SetPitch(float pitch);

    void ClearModulations();
    void AddPitchModulation(float pitch);

    float GetAbsolute();

private:
    float m_hertz = 440;
    float m_pitchBase = 0.0;
    float m_pitchModulation = 0.0;
    static inline float m_notesPerOctave = 12.0;
};