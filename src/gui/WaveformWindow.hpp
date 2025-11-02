// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "ImGuiDefinitions.hpp"
#include "imgui.h"
#include "audio/core/Waveform.hpp"
#include <GLFW/glfw3.h>
#include <vector>

class WaveformWindow {
public:
    WaveformWindow(WaveformInfo::Type waveformType);

    // render in imgui
    void Render(WaveformInfo::Type waveformType);

    void InitTexture();

private:
    ImVec2 ConfigureWindow() const;

    void UpdateTexture();

    // Size of texture in pixels
    static const int TEXTURE_HEIGHT = 64;
    static const int TEXTURE_WIDTH = 128;
    
    // Make sure to fit entire image neatly within window
    static const int IMAGE_PADDING_BOTTOM = 40;
    static const int IMAGE_PADDING_RIGHT = 16;

    GLuint m_waveformTex = 0;
    WaveformInfo::Type m_waveformType;

    static inline const std::array<unsigned char, 3> BG_COLOR{5, 9, 19};
    static inline const std::array<unsigned char, 3> HIGHLIGHT_COLOR{107, 201, 255};
};