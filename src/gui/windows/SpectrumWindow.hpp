// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <vector>

namespace GUI::Window {

class SpectrumWindow {
public:
    static const int WINDOW_WIDTH = 528;
    static const int WINDOW_HEIGHT = 140;

    // Push the latest FFT magnitudes
    void PushMagnitudes(const std::vector<float>& magnitudes);

    // Draw inside ImGui
    void Render();

private:
    void ConfigureWindow() const;

    std::vector<float> m_magnitudes;
    std::vector<float> m_smoothed; // smoothing for stability

    const float SMOOTHING_FACTOR = 0.65f; // lower = faster response
};

} // namespace GUI::Window

