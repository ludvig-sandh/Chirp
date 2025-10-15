#pragma once

#include "imgui.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <vector>

class Spectrogram {
public:
    // update per audio block
    void PushColumn(const std::vector<float>& magnitudes);

    // render in imgui
    void Show();

    // create texture once. Must be done after GL context has been initialized
    void InitTexture();

    void ReallocateTexture();

private:
    static void MagnitudeToRGB(float mag, unsigned char& r, unsigned char& g, unsigned char& b);

    // Dimensions of spectrogram UI element
    static const int UI_SPEC_HEIGHT = 256;
    static const int UI_SPEC_WIDTH = 512;

    GLuint m_spectrogramTex = 0;
    int m_currentColumn = 0;
    int m_specHeight = 0; // dynamically set from magnitudes.size()
    const int m_specWidth = 256; // number of time slices visible
    std::vector<std::vector<float>> m_magnitudeHistory;
};