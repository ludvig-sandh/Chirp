// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "imgui.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <array>

class SpectrogramWindow {
public:
    // Dimensions of spectrogram window
    static const int WINDOW_HEIGHT = 294;
    static const int WINDOW_WIDTH = 528;

    // update per audio block
    void PushColumn(const std::vector<float>& magnitudes);

    // render in imgui
    void Render();

    // create texture once. Must be done after GL context has been initialized
    void InitTexture();

    void ReallocateTexture();

private:
    void ConfigureWindow() const;

    // Turns a magnitude in range [0, 1] into an array of RGBA colors
    static std::array<unsigned char, 4> MagnitudeToRGBA(float mag);

    // Dimensions of spectrogram image element
    static const int UI_IMAGE_HEIGHT = 256;
    static const int UI_IMAGE_WIDTH = 512;

    GLuint m_spectrogramTex = 0;
    int m_currentColumn = 0;
    int m_specHeight = 0; // dynamically set from magnitudes size
    static constexpr int SPEC_WIDTH = 256; // number of time slices visible
    std::vector<std::vector<float>> m_magnitudeHistory;
};
