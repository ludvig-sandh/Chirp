// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/LevelsHistory.hpp"

#include <algorithm>

void LevelsHistory::Add(float db) {
    m_history.push_back(db);
    if (m_history.size() > s_historySize) {
        m_history.pop_front();
    }

    float currentMax = GetMax();
    m_maxWithDecay -= s_decay;
    m_maxWithDecay = std::max(m_maxWithDecay, 0.0f);
    if (currentMax > m_maxWithDecay) {
        m_maxWithDecay = currentMax;
    }
}

float LevelsHistory::GetMaxWithDecay() {
    return m_maxWithDecay;
}

float LevelsHistory::GetMax() {
    auto maxIt = std::max_element(m_history.begin(), m_history.end());
    if (maxIt == m_history.end()) {
        return 0.0f;
    }

    return *maxIt;
}