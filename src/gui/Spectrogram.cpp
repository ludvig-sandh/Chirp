
#include "Spectrogram.hpp"

#include <algorithm>

// update per audio block
void Spectrogram::PushColumn(const std::vector<float>& magnitudes) {
    // Detect size change → reallocate
    if (m_specHeight != (int)magnitudes.size()) {
        m_specHeight = (int)magnitudes.size();
        ReallocateTexture();  // new helper
    }

    // Ensure m_magnitudeHistory matches new height
    if (m_magnitudeHistory.empty() || 
        (int)m_magnitudeHistory[0].size() != m_specHeight) {
        m_magnitudeHistory.assign(SPEC_WIDTH, std::vector<float>(m_specHeight, 0.0f));
    }

    // Store new magnitudes
    m_magnitudeHistory[m_currentColumn] = magnitudes;
    m_currentColumn = (m_currentColumn + 1) % SPEC_WIDTH;

    // Prepare pixels (dynamic height!)
    std::vector<unsigned char> pixels(SPEC_WIDTH * m_specHeight * 3, 0);
    for (int column_idx = 0; column_idx < SPEC_WIDTH; column_idx++) {
        int x = (m_currentColumn + column_idx) % SPEC_WIDTH;
        for (int y = 0; y < m_specHeight; y++) {
            float mag = m_magnitudeHistory[x][y];
            unsigned char c = (unsigned char)std::clamp(mag * 255.0f, 0.0f, 255.0f);
            int pixel_idx = ((m_specHeight - y - 1) * SPEC_WIDTH + column_idx) * 3;
            pixels[pixel_idx + 0] = c;
            pixels[pixel_idx + 1] = c;
            pixels[pixel_idx + 2] = c;
        }
    }

    // Upload the texture
    glBindTexture(GL_TEXTURE_2D, m_spectrogramTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SPEC_WIDTH, m_specHeight,
                    GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
}

// render in imgui
void Spectrogram::Show() {
    if (m_specHeight == 0) return;

    ImGui::Begin("Spectrogram");
    ImGui::Image((ImTextureID)(intptr_t)m_spectrogramTex,
                 ImVec2(SPEC_WIDTH, m_specHeight));
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
                 SPEC_WIDTH, m_specHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}