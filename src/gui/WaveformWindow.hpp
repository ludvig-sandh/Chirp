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
    WaveformWindow(WaveformInfo::Type waveformType, const std::string& windowName);

    // render in imgui. comesFirst tells us if this should be rendered in the first or second slot
    void Render(WaveformInfo::Type waveformType, bool comesFirst, bool isOn);

    void InitTexture();

private:
    ImVec2 ConfigureWindow(bool comesFirst) const;

    void UpdateTexture();

    // Size of texture in pixels
    static const int TEXTURE_HEIGHT = 256;
    static const int TEXTURE_WIDTH = 512;
    
    // Make sure to fit entire image neatly within window
    static const int IMAGE_PADDING_BOTTOM = 40;
    static const int IMAGE_PADDING_RIGHT = 16;

    GLuint m_waveformTex = 0;
    WaveformInfo::Type m_waveformType;
    std::string m_windowName;

    static inline const float ON_OPACITY = 1.0f;
    static inline const float OFF_OPACITY = 0.2f;
};