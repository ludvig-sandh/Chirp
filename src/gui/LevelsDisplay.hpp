#pragma once

#include "imgui.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <vector>
#include <array>
#include <utility>
#include <deque>

class LevelsDisplay {
public:
    void UpdateLevels(const std::pair<float, float>& levels);

    // Render in imgui
    void Show();
    
    // Create texture once. Must be done after GL context has been initialized
    void InitTexture();

private:
    void SetPixelHelper(std::vector<unsigned char>& pixels, int x, int y, const std::array<unsigned char, 3>& rgb);
    
    // Dimensions of spectrogram UI element
    static const int s_UIHeight = 256;
    static const int s_UIWidth = 32;

    static const int s_texHeight = 64;
    static const int s_texWidth = 7;
    static const size_t maxVolumeBars = s_texHeight - 2; // Leave 1 pixel gap on top and bottom

    static const std::array<unsigned char, 3> s_bgColor;
    static const std::array<unsigned char, 3> s_highlightColor;

    GLuint m_levelsTex = 0;
};