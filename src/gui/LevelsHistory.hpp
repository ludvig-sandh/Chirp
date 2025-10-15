#pragma once

#include <deque>

class LevelsHistory {
public:
    void Add(float db);

    // Returns the recent max volume in dB that decays over time
    float GetMaxWithDecay();

private:
    float GetMax();

    float m_maxWithDecay = 0.0;
    std::deque<float> m_history;

    float s_decay = 0.02;
    size_t s_historySize = 30;
};
