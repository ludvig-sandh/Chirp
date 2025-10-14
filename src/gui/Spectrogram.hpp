#pragma once

#include "imgui.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <vector>
#include <iostream>

class Spectrogram {
public:
    Spectrogram() : m_magnitudeHistory(SPEC_WIDTH, std::vector<float>(SPEC_HEIGHT, 0.0)) {}

    // update per audio block
    void PushColumn(const std::vector<float>& magnitudes);

    // render in imgui
    void Show();

    // create texture once. Must be done after GL context has been initialized
    void InitTexture();

    void ReallocateTexture();

private:
    // Dimensions of spectrogram
    static const int FFT_SIZE = 512;
    static const int SPEC_HEIGHT = FFT_SIZE / 2;
    static const int SPEC_WIDTH = 512; // number of time slices visible

    GLuint m_spectrogramTex = 0;
    int m_currentColumn = 0;
    int m_specHeight = 0; // dynamically set from magnitudes.size()
    std::vector<std::vector<float>> m_magnitudeHistory;
};