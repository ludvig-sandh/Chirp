// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <vector>

class SpectrumWindow {
public:
    static const int WINDOW_WIDTH = 528;
    static const int WINDOW_HEIGHT = 140;

    // push the latest FFT magnitudes (normalized 0..1 recommended)
    void PushMagnitudes(const std::vector<float>& magnitudes);

    // draw inside ImGui
    void Render();

private:
    void ConfigureWindow() const;

    std::vector<float> m_magnitudes;
    std::vector<float> m_smoothed; // smoothing for stability

    const float SMOOTHING_FACTOR = 0.65f; // lower = faster response
};
