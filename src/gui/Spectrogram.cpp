// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/Spectrogram.hpp"
#include "gui/LevelsWindow.hpp"
#include "gui/GUIConstants.hpp"

#include <cmath>
#include <algorithm>

// update per audio block
void Spectrogram::PushColumn(const std::vector<float>& magnitudes) {
    // Detect size change and reallocate magnitude history if needed
    if (m_specHeight != std::ssize(magnitudes)) {
        m_specHeight = std::ssize(magnitudes);
        // Reallocate magnitude history buffer
        m_magnitudeHistory.assign(SPEC_WIDTH, std::vector<float>(m_specHeight, 0.0f));
        ReallocateTexture();
    }

    // Store new magnitudes into the current column
    m_magnitudeHistory[m_currentColumn] = magnitudes;
    m_currentColumn = (m_currentColumn + 1) % SPEC_WIDTH;

    // Prepare pixels
    std::vector<unsigned char> pixels(SPEC_WIDTH * m_specHeight * 4, 0);
    for (int column_idx = 0; column_idx < SPEC_WIDTH; column_idx++) {
        int x = (m_currentColumn + column_idx) % SPEC_WIDTH;
        for (int y = 0; y < m_specHeight; y++) {
            float mag = std::clamp(m_magnitudeHistory[x][y], 0.0f, 1.0f);
            std::array<unsigned char, 4> rgba = Spectrogram::MagnitudeToRGBA(mag);

            int pixel_idx = ((m_specHeight - y - 1) * SPEC_WIDTH + column_idx) * 4;
            pixels[pixel_idx + 0] = rgba[0];
            pixels[pixel_idx + 1] = rgba[1];
            pixels[pixel_idx + 2] = rgba[2];
            pixels[pixel_idx + 3] = rgba[3];
        }
    }

    // Upload to OpenGL texture
    glBindTexture(GL_TEXTURE_2D, m_spectrogramTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SPEC_WIDTH, m_specHeight,
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
}

void Spectrogram::Render() {
    if (m_specHeight == 0) return;

    ConfigureWindow();
    ImGui::Image((ImTextureID)(intptr_t)m_spectrogramTex,
                 ImVec2(UI_IMAGE_WIDTH, UI_IMAGE_HEIGHT));
    ImGui::End();
}

void Spectrogram::ConfigureWindow() const {
    // Get viewport (the main window area)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(
        ImVec2(viewport->Pos.x + viewport->Size.x - LevelsWindow::WINDOW_WIDTH - WINDOW_WIDTH, viewport->Pos.y),
        ImGuiCond_Always
    );
    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin("Spectrogram", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::SeparatorText("Spectrogram");
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 SPEC_WIDTH, m_specHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

std::array<unsigned char, 4> Spectrogram::MagnitudeToRGBA(float mag) {
    mag *= 2;
    mag = std::clamp(mag, 0.0f, 1.0f);

    float r_f, g_f, b_f;
    float a_f = 255.0f;

    if (mag < 0.5f) {
        // interpolate c0 -> c1
        float t = mag / 0.5f;
        r_f = GUIConstants::Colors::MIDTONE[0];
        g_f = GUIConstants::Colors::MIDTONE[1];
        b_f = GUIConstants::Colors::MIDTONE[2];
        a_f = t * GUIConstants::Colors::MIDTONE[3];
    } else {
        // interpolate c1 -> c2
        float t = (mag - 0.5f) / 0.5f;
        r_f = (1 - t) * GUIConstants::Colors::MIDTONE[0] + t * GUIConstants::Colors::HIGHLIGHT[0];
        g_f = (1 - t) * GUIConstants::Colors::MIDTONE[1] + t * GUIConstants::Colors::HIGHLIGHT[1];
        b_f = (1 - t) * GUIConstants::Colors::MIDTONE[2] + t * GUIConstants::Colors::HIGHLIGHT[2];
        a_f = (1 - t) * GUIConstants::Colors::MIDTONE[3] + t * GUIConstants::Colors::HIGHLIGHT[3];
    }
    
    return {
        static_cast<unsigned char>(std::clamp(r_f, 0.0f, 255.0f)),
        static_cast<unsigned char>(std::clamp(g_f, 0.0f, 255.0f)),
        static_cast<unsigned char>(std::clamp(b_f, 0.0f, 255.0f)),
        static_cast<unsigned char>(std::clamp(a_f, 0.0f, 255.0f)),
    };
}
