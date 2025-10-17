#pragma once

#include "Frequency.hpp"
#include <random>

// Base class for representing waveforms, such as sine waves or more complex waves  
class Waveform {
public:
    // Returns the sample value at a specific offset in the waveform in the range [0, 1]
    virtual float GetSampleAt(float offset) = 0;
};

class Sine final : public Waveform {
public:
    float GetSampleAt(float offset) override;
};

class Square final : public Waveform {
public:
    float GetSampleAt(float offset) override;
};

class Saw final : public Waveform {
public:
    float GetSampleAt(float offset) override;
};

class WhiteNoise final : public Waveform {
public:
    WhiteNoise() : m_gen(1337), m_dist(-1.0f, 1.0f) {}
    float GetSampleAt(float offset) override;
private:
    std::mt19937 m_gen;
    std::uniform_real_distribution<float> m_dist;
};