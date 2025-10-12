#include "RandomLFO.hpp"

RandomSignalGenerator::RandomSignalGenerator(Frequency freq) {
    numSamplesPerPeriod = SAMPLE_RATE / freq.GetAbsolute();
    GenerateRandValueHelper();
    lastRandValue = GenerateRandValueHelper();
    nextRandValue = GenerateRandValueHelper();
}

float RandomSignalGenerator::GetNextSample() {
    float t = static_cast<float>(sampleCount) / numSamplesPerPeriod;
    float toReturn = CubicInterpolate(lastRandValue, nextRandValue, t);

    sampleCount++;
    if (sampleCount == numSamplesPerPeriod) {
        sampleCount = 0;
        lastRandValue = nextRandValue;
        nextRandValue = GenerateRandValueHelper();
    }

    return toReturn;
}

float RandomSignalGenerator::GenerateRandValueHelper() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float RandomSignalGenerator::CubicInterpolate(float y0, float y1, float t, float m0, float m1) {
    float t2 = t * t;
    float t3 = t2 * t;

    float h00 = 2*t3 - 3*t2 + 1;
    float h10 = t3 - 2*t2 + t;
    float h01 = -2*t3 + 3*t2;
    float h11 = t3 - t2;

    return h00*y0 + h10*m0 + h01*y1 + h11*m1;
}