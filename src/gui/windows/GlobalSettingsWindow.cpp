// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/windows/GlobalSettingsWindow.hpp"

#include "gui/windows/SpectrogramWindow.hpp"
#include "gui/windows/LevelsWindow.hpp"

namespace GUI::Window {

void GlobalSettingsWindow::Render(Audio::Preset::AudioPreset& preset) const {
    ConfigureWindow();
    DrawGlobalSettings(preset);
    ImGui::End();
}

void GlobalSettingsWindow::ConfigureWindow() const {
    // Get viewport (the main window area)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(
        ImVec2(viewport->Pos.x, viewport->Pos.y),
        ImGuiCond_Always
    );

    const float windowWidth = viewport->Size.x - SpectrogramWindow::WINDOW_WIDTH - LevelsWindow::WINDOW_WIDTH;
    ImGui::SetNextWindowSize(ImVec2(windowWidth, WINDOW_HEIGHT), ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin("Global settings", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse);
}

void GlobalSettingsWindow::DrawGlobalSettings(Audio::Preset::AudioPreset& preset) const {
    ImGui::SeparatorText("Global settings");

    float volumeTemp = preset.synthMasterVolume.load();
    ImGui::SliderFloat("Master volume", &volumeTemp, 0.0f, 1.0f);
    preset.synthMasterVolume.store(volumeTemp);
}

} // namespace GUI::Window
