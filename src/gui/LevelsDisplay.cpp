// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "LevelsDisplay.hpp"

#include "imgui.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>

const std::array<unsigned char, 3> LevelsDisplay::s_bgColor = {5, 9, 19};
const std::array<unsigned char, 3> LevelsDisplay::s_secondaryColor = {31, 64, 132};
const std::array<unsigned char, 3> LevelsDisplay::s_highlightColor = {107, 201, 255};

void LevelsDisplay::SetPixelHelper(std::vector<unsigned char>& pixels, int x, int y, const std::array<unsigned char, 3>& rgb) {
    size_t pixelIndex = (s_texHeight - y - 1) * s_texWidth + x;
    std::memcpy(&pixels[pixelIndex * 3], rgb.data(), 3 * sizeof(unsigned char));
}

void LevelsDisplay::UpdateLevels(const AudioFrame& levels) {
    // Convert to dB normalized in range [0, 1]
    double dbL = 20.0 * std::log10(levels.left);
    double dbR = 20.0 * std::log10(levels.right);
    const double minDb = -100.0;
    const double maxDb = 0.0;
    double normL = std::clamp((dbL - minDb) / (maxDb - minDb), 0.0, 1.0);
    double normR = std::clamp((dbR - minDb) / (maxDb - minDb), 0.0, 1.0);

    m_leftHistory.Add(normL);
    m_rightHistory.Add(normR);

    // Init pixels with background colors
    std::vector<unsigned char> pixels(s_texWidth * s_texHeight * 3);
    for (int x = 0; x < s_texWidth; x++) {
        for (int y = 0; y < s_texHeight; y++) {
            SetPixelHelper(pixels, x, y, s_bgColor);
        }
    }

    // Find out how tall the bars should be in the texture
    size_t numVolumeBarsLeft = static_cast<size_t>(normL * maxVolumeBars);
    size_t numVolumeBarsRight = static_cast<size_t>(normR * maxVolumeBars);
    size_t numRecentMaxVolumeBarsLeft = static_cast<size_t>(m_leftHistory.GetMaxWithDecay() * maxVolumeBars);
    size_t numRecentMaxVolumeBarsRight = static_cast<size_t>(m_rightHistory.GetMaxWithDecay() * maxVolumeBars);

    // Left channel
    for (size_t bar = 0; bar < numRecentMaxVolumeBarsLeft; bar++) {
        size_t y = bar + 1;
        if (bar < numVolumeBarsLeft) {
            // Current vol
            SetPixelHelper(pixels, 1, y, s_highlightColor);
            SetPixelHelper(pixels, 2, y, s_highlightColor);
        }else {
            // Recent max
            SetPixelHelper(pixels, 1, y, s_secondaryColor);
            SetPixelHelper(pixels, 2, y, s_secondaryColor);
        }
    }

    // Right channel
    for (size_t bar = 0; bar < numRecentMaxVolumeBarsRight; bar++) {
        size_t y = bar + 1;
        if (bar < numVolumeBarsRight) {
            // Current vol
            SetPixelHelper(pixels, 4, y, s_highlightColor);
            SetPixelHelper(pixels, 5, y, s_highlightColor);
        }else {
            // Recent max
            SetPixelHelper(pixels, 4, y, s_secondaryColor);
            SetPixelHelper(pixels, 5, y, s_secondaryColor);
        }
    }

    // Upload to OpenGL texture
    glBindTexture(GL_TEXTURE_2D, m_levelsTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s_texWidth, s_texHeight,
                    GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
}

void LevelsDisplay::Show() {
    ImGui::Begin("Lvl");
    ImGui::Image((ImTextureID)(intptr_t)m_levelsTex,
                 ImVec2(s_UIWidth, s_UIHeight));
    ImGui::End();
}

void LevelsDisplay::InitTexture() {
    glGenTextures(1, &m_levelsTex);
    glBindTexture(GL_TEXTURE_2D, m_levelsTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 s_texWidth, s_texHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}