#pragma once

#include "LFO.hpp"
#include "Frequency.hpp"
#include "AudioEngine.hpp"

class RandomSignalGenerator: LFO {
public:
    RandomSignalGenerator(Frequency freq);

    float GetNextSample() override;

private:
    float GenerateRandValueHelper();

    // Simple cubic Hermite interpolation between y0 and y1
    // t = fraction [0,1]
    // Optional tangents m0 and m1; default to 0 for simple smoothness
    float CubicInterpolate(float y0, float y1, float t, float m0 = 0.0, float m1 = 0.0);

    float lastRandValue;
    float nextRandValue;
    int sampleCount = 0;
    int numSamplesPerPeriod;
};