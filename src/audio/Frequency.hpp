#pragma once

#include <algorithm>
#include <cmath>

class Frequency {
public:
    Frequency(float hertz);

    void SetPitch(float pitch);

    float GetAbsolute();

private:
    float hertz;
    float pitch;
    static inline float notesPerOctave = 12.0;
};