// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/LevelsDisplay.hpp"
#include "gui/GUIConstants.hpp"

#include <cstring>
#include <cmath>
#include <algorithm>

void LevelsDisplay::SetPixelHelper(std::vector<unsigned char>& pixels, int x, int y, const std::array<unsigned char, 4>& rgba) {
    size_t pixelIndex = 4 * ((TEXTURE_HEIGHT - y - 1) * TEXTURE_WIDTH + x);
    pixels[pixelIndex] = rgba[0];
    pixels[pixelIndex + 1] = rgba[1];
    pixels[pixelIndex + 2] = rgba[2];
    pixels[pixelIndex + 3] = rgba[3];
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

    // Init pixels with clear color
    std::vector<unsigned char> pixels(TEXTURE_WIDTH * TEXTURE_HEIGHT * 4, 0);

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
            SetPixelHelper(pixels, 1, y, GUIConstants::Colors::HIGHLIGHT);
            SetPixelHelper(pixels, 2, y, GUIConstants::Colors::HIGHLIGHT);
        }else {
            // Recent max
            SetPixelHelper(pixels, 1, y, GUIConstants::Colors::MIDTONE);
            SetPixelHelper(pixels, 2, y, GUIConstants::Colors::MIDTONE);
        }
    }

    // Right channel
    for (size_t bar = 0; bar < numRecentMaxVolumeBarsRight; bar++) {
        size_t y = bar + 1;
        if (bar < numVolumeBarsRight) {
            // Current vol
            SetPixelHelper(pixels, 4, y, GUIConstants::Colors::HIGHLIGHT);
            SetPixelHelper(pixels, 5, y, GUIConstants::Colors::HIGHLIGHT);
        }else {
            // Recent max
            SetPixelHelper(pixels, 4, y, GUIConstants::Colors::MIDTONE);
            SetPixelHelper(pixels, 5, y, GUIConstants::Colors::MIDTONE);
        }
    }

    // Upload to OpenGL texture
    glBindTexture(GL_TEXTURE_2D, m_levelsTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT,
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
}

void LevelsDisplay::Render() {
    ConfigureWindow();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + IMAGE_START_X); // shift image slightly to the right
    ImGui::Image((ImTextureID)(intptr_t)m_levelsTex,
                 ImVec2(IMAGE_WIDTH, IMAGE_HEIGHT));

    ImGui::End();
}

void LevelsDisplay::ConfigureWindow() const {
    // Get viewport (the main window area)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Force window to bottom, full width
    ImGui::SetNextWindowPos(
        ImVec2(viewport->Pos.x + viewport->Size.x - WINDOW_WIDTH, viewport->Pos.y),
        ImGuiCond_Always
    );
    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin("Levels", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::SeparatorText("Levels");
}

void LevelsDisplay::InitTexture() {
    glGenTextures(1, &m_levelsTex);
    glBindTexture(GL_TEXTURE_2D, m_levelsTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 TEXTURE_WIDTH, TEXTURE_HEIGHT, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}