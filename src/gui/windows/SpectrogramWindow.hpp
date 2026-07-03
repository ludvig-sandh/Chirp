// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "gui/windows/helper/SpectrogramRenderer.hpp"
#include "imgui.h"
#include <vector>

namespace GUI::Window {

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

    // release GPU resources before the OpenGL context is destroyed
    void Shutdown();

private:
    void ConfigureWindow() const;

    // Dimensions of spectrogram image element
    static const int UI_IMAGE_HEIGHT = 256;
    static const int UI_IMAGE_WIDTH = 512;

    int m_currentColumn = 0;
    int m_specHeight = 0; // dynamically set from magnitudes size
    static constexpr int SPEC_WIDTH = 256; // number of time slices visible
    Helper::SpectrogramRenderer m_renderer;
};

} // namespace GUI::Window
