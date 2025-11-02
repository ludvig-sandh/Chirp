// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "ImGuiDefinitions.hpp"
#include "imgui.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "gui/LevelsHistory.hpp"
#include "engine/AudioBackend.hpp"
#include <vector>
#include <array>
#include <utility>
#include <deque>

class LevelsDisplay {
public:
    static const int WINDOW_HEIGHT = 294;
    static const int WINDOW_WIDTH = 80;

    void UpdateLevels(const AudioFrame& levels);

    // Render in imgui
    void Render();
    
    // Create texture once. Must be done after GL context has been initialized
    void InitTexture();

private:
    void ConfigureWindow() const;

    void SetPixelHelper(std::vector<unsigned char>& pixels, int x, int y, const std::array<unsigned char, 3>& rgb);

    // Size of texture in pixels
    static const int TEXTURE_HEIGHT = 64;
    static const int TEXTURE_WIDTH = 7;
    
    // Size that the texture will show up on screen with
    static const int IMAGE_HEIGHT = 256;
    static const int IMAGE_WIDTH = 32;
    static inline const float IMAGE_START_X = 16.0f; // Offset in x from left window border

    static const size_t maxVolumeBars = TEXTURE_HEIGHT - 2; // Leave 1 pixel gap on top and bottom

    static const std::array<unsigned char, 3> s_bgColor;
    static const std::array<unsigned char, 3> s_secondaryColor;
    static const std::array<unsigned char, 3> s_highlightColor;

    LevelsHistory m_leftHistory;
    LevelsHistory m_rightHistory;

    GLuint m_levelsTex = 0;
};