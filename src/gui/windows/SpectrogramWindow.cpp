// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/windows/SpectrogramWindow.hpp"
#include "gui/windows/LevelsWindow.hpp"

namespace GUI::Window {

// update per audio block
void SpectrogramWindow::PushColumn(const std::vector<float>& magnitudes) {
    if (!m_renderer.Init()) {
        return;
    }

    if (m_specHeight != std::ssize(magnitudes)) {
        m_specHeight = std::ssize(magnitudes);
        m_currentColumn = 0;
        m_renderer.Resize(SPEC_WIDTH, m_specHeight);
    }

    if (m_specHeight == 0) {
        return;
    }

    m_renderer.PushColumn(magnitudes, m_currentColumn);
    m_currentColumn = (m_currentColumn + 1) % SPEC_WIDTH;
}

void SpectrogramWindow::Render() {
    if (m_specHeight == 0) return;

    ConfigureWindow();
    ImGui::Image((ImTextureID)(intptr_t)m_renderer.TextureId(),
                 ImVec2(UI_IMAGE_WIDTH, UI_IMAGE_HEIGHT));
    ImGui::End();
}

void SpectrogramWindow::ConfigureWindow() const {
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

void SpectrogramWindow::InitTexture() {
    m_renderer.Init();
}

void SpectrogramWindow::Shutdown() {
    m_renderer.Shutdown();
}

} // namespace GUI::Window
