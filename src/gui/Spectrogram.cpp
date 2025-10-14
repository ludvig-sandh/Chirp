
#include "Spectrogram.hpp"

// update per audio block
void Spectrogram::PushColumn(const std::vector<float>& magnitudes) {
    // Store into rolling buffer
    m_magnitudeHistory[m_currentColumn] = magnitudes;

    m_currentColumn = (m_currentColumn + 1) % SPEC_WIDTH;

    // Convert to color image (grayscale example)
    std::vector<unsigned char> pixels(SPEC_WIDTH * SPEC_HEIGHT * 3, 255);
    for (int column_idx = 0; column_idx < SPEC_WIDTH; column_idx++) {
        int x = (m_currentColumn + column_idx) % SPEC_WIDTH;

        for (int y = 0; y < SPEC_HEIGHT; y++) {
            float mag = m_magnitudeHistory[x][y];
            unsigned char c = (unsigned char)(mag * 255);
            int pixel_idx = ((SPEC_HEIGHT - y - 1) * SPEC_WIDTH + column_idx) * 3;
            pixels[pixel_idx] = c;
            pixels[pixel_idx + 1] = c;
            pixels[pixel_idx + 2] = c;
        }
    }

    // Upload texture
    glBindTexture(GL_TEXTURE_2D, m_spectrogramTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SPEC_WIDTH, SPEC_HEIGHT,
                    GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
}

// render in imgui
void Spectrogram::Show() {
    ImGui::Begin("Spectrogram");
    ImGui::Image((ImTextureID)(intptr_t)m_spectrogramTex, ImVec2(SPEC_WIDTH, SPEC_HEIGHT));
    ImGui::End();
}

// create texture once
void Spectrogram::InitTexture() {
    glGenTextures(1, &m_spectrogramTex);
    glBindTexture(GL_TEXTURE_2D, m_spectrogramTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SPEC_WIDTH, SPEC_HEIGHT, 0,
                GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
