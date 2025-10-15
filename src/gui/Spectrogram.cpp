
#include "Spectrogram.hpp"

#include <cmath>
#include <algorithm>

// update per audio block
void Spectrogram::PushColumn(const std::vector<float>& magnitudes) {
    // Detect size change and reallocate magnitude history if needed
    if (m_specHeight != (int)magnitudes.size()) {
        m_specHeight = (int)magnitudes.size();
        // Reallocate magnitude history buffer
        m_magnitudeHistory.assign(m_specWidth, std::vector<float>(m_specHeight, 0.0f));
        ReallocateTexture();
    }

    // Store new magnitudes into the current column
    m_magnitudeHistory[m_currentColumn] = magnitudes;
    m_currentColumn = (m_currentColumn + 1) % m_specWidth;

    // Prepare pixels (dynamic height!)
    std::vector<unsigned char> pixels(m_specHeight * m_specHeight * 3, 0);
    for (int column_idx = 0; column_idx < m_specWidth; column_idx++) {
        int x = (m_currentColumn + column_idx) % m_specWidth;
        for (int y = 0; y < m_specHeight; y++) {
            float mag = std::clamp(m_magnitudeHistory[x][y], 0.0f, 1.0f);
            unsigned char r, g, b;
            Spectrogram::MagnitudeToRGB(mag, r, g, b);

            int pixel_idx = ((m_specHeight - y - 1) * m_specWidth + column_idx) * 3;
            pixels[pixel_idx + 0] = r;
            pixels[pixel_idx + 1] = g;
            pixels[pixel_idx + 2] = b;
        }
    }

    // Upload to OpenGL texture
    glBindTexture(GL_TEXTURE_2D, m_spectrogramTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_specWidth, m_specHeight,
                    GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
}

// render in imgui
void Spectrogram::Show() {
    if (m_specHeight == 0) return;

    ImGui::Begin("Spectrogram");
    ImGui::Image((ImTextureID)(intptr_t)m_spectrogramTex,
                 ImVec2(UI_SPEC_WIDTH, UI_SPEC_HEIGHT));
    ImGui::End();
}

void Spectrogram::InitTexture() {
    ReallocateTexture();
}

void Spectrogram::ReallocateTexture() {
    if (m_spectrogramTex) {
        glDeleteTextures(1, &m_spectrogramTex);
    }

    glGenTextures(1, &m_spectrogramTex);
    glBindTexture(GL_TEXTURE_2D, m_spectrogramTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 m_specWidth, m_specHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Spectrogram::MagnitudeToRGB(float mag, unsigned char& r, unsigned char& g, unsigned char& b)
{
    mag *= 2;
    mag = std::clamp(mag, 0.0f, 1.0f);

    // Define 3 anchor colors (in linear RGB)
    const float c0[3] = { 0x05 / 255.0f, 0x09 / 255.0f, 0x13 / 255.0f }; // very dark
    const float c1[3] = { 0x11 / 255.0f, 0x26 / 255.0f, 0x5C / 255.0f }; // base (#11265C)
    const float c2[3] = { 0x6B / 255.0f, 0xC9 / 255.0f, 0xFF / 255.0f }; // highlight

    float r_f, g_f, b_f;

    if (mag < 0.5f) {
        // interpolate c0 → c1
        float t = mag / 0.5f;
        r_f = (1 - t) * c0[0] + t * c1[0];
        g_f = (1 - t) * c0[1] + t * c1[1];
        b_f = (1 - t) * c0[2] + t * c1[2];
    } else {
        // interpolate c1 → c2
        float t = (mag - 0.5f) / 0.5f;
        r_f = (1 - t) * c1[0] + t * c2[0];
        g_f = (1 - t) * c1[1] + t * c2[1];
        b_f = (1 - t) * c1[2] + t * c2[2];
    }

    // // Optional: apply slight brightness curve to enhance contrast
    // float gamma = 0.8f; // lower = more contrast
    // r_f = std::pow(r_f, gamma);
    // g_f = std::pow(g_f, gamma);
    // b_f = std::pow(b_f, gamma);

    r = (unsigned char)(std::clamp(r_f, 0.0f, 1.0f) * 255);
    g = (unsigned char)(std::clamp(g_f, 0.0f, 1.0f) * 255);
    b = (unsigned char)(std::clamp(b_f, 0.0f, 1.0f) * 255);
}
