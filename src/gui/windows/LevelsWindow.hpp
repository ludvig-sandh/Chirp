// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "gui/ImGuiDefinitions.hpp"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include "gui/windows/helper/LevelsHistory.hpp"
#include "audio/engine/AudioBackend.hpp"
#include <vector>
#include <array>
#include <utility>
#include <deque>

class LevelsWindow {
public:
    static const int WINDOW_HEIGHT = 294;
    static const int WINDOW_WIDTH = 80;

    void UpdateLevels(const Audio::Engine::AudioFrame& levels);

    // Render in imgui
    void Render();
    
    // Create texture once. Must be done after GL context has been initialized
    void InitTexture();

private:
    void ConfigureWindow() const;

    void SetPixelHelper(std::vector<unsigned char>& pixels, int x, int y, const std::array<unsigned char, 4>& rgb);

    // Size of texture in pixels
    static const int TEXTURE_HEIGHT = 64;
    static const int TEXTURE_WIDTH = 7;
    
    // Size that the texture will show up on screen with
    static const int IMAGE_HEIGHT = 256;
    static const int IMAGE_WIDTH = 32;
    static inline const float IMAGE_START_X = 16.0f; // Offset in x from left window border

    static const int maxVolumeBars = TEXTURE_HEIGHT - 2; // Leave 1 pixel gap on top and bottom

    LevelsHistory m_leftHistory;
    LevelsHistory m_rightHistory;

    GLuint m_levelsTex = 0;
};