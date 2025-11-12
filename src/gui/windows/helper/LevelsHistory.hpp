// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <deque>

// Keeps track of audio levels over time so that the LevelsWindow can display both current and recent audio levels. 
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
    int s_historySize = 30;
};
