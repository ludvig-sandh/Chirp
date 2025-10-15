#include "LevelsDisplay.hpp"

#include "imgui.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>

const std::array<unsigned char, 3> LevelsDisplay::s_bgColor = {5, 9, 19};
const std::array<unsigned char, 3> LevelsDisplay::s_highlightColor = {107, 201, 255};

void LevelsDisplay::SetPixelHelper(std::vector<unsigned char>& pixels, int x, int y, const std::array<unsigned char, 3>& rgb) {
    size_t pixelIndex = (s_texHeight - y - 1) * s_texWidth + x;
    std::memcpy(&pixels[pixelIndex * 3], rgb.data(), 3 * sizeof(unsigned char));
}

void LevelsDisplay::UpdateLevels(const std::pair<float, float>& levels) {
    // Convert to dB normalized in range [0, 1]
    double dbL = 20.0 * std::log10(levels.first);
    double dbR = 20.0 * std::log10(levels.second);
    const double minDb = -100.0;
    const double maxDb = 0.0;
    double normL = std::clamp((dbL - minDb) / (maxDb - minDb), 0.0, 1.0);
    double normR = std::clamp((dbR - minDb) / (maxDb - minDb), 0.0, 1.0);

    std::vector<unsigned char> pixels(s_texWidth * s_texHeight * 3);

    // Init pixels with background colors
    for (int x = 0; x < s_texWidth; x++) {
        for (int y = 0; y < s_texHeight; y++) {
            SetPixelHelper(pixels, x, y, s_bgColor);
        }
    }

    size_t numVolumeBarsLeft = static_cast<size_t>(normL * maxVolumeBars);
    size_t numVolumeBarsRight = static_cast<size_t>(normR * maxVolumeBars);

    // Left channel
    for (size_t y = 1; y < numVolumeBarsLeft + 1; y++) {
        SetPixelHelper(pixels, 1, y, s_highlightColor);
        SetPixelHelper(pixels, 2, y, s_highlightColor);
    }

    // Right channel
    for (size_t y = 1; y < numVolumeBarsRight + 1; y++) {
        SetPixelHelper(pixels, 4, y, s_highlightColor);
        SetPixelHelper(pixels, 5, y, s_highlightColor);
    }

    // Upload to OpenGL texture
    glBindTexture(GL_TEXTURE_2D, m_levelsTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s_texWidth, s_texHeight,
                    GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
}

void LevelsDisplay::Show() {
    ImGui::Begin("Audio levels");
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