// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/PresetLoaderWindow.hpp"

#include "gui/Spectrogram.hpp"
#include "gui/LevelsDisplay.hpp"
#include "gui/GlobalSettingsWindow.hpp"
#include "preset/AudioPresetSerialization.hpp"
#include "preset/BuiltInPresetsLoader.hpp"

void PresetLoaderWindow::Render(AudioPreset& preset) const {
    ConfigureWindow();
    DrawPresetLoader(preset);
    ImGui::End();
}

void PresetLoaderWindow::ConfigureWindow() const {
    // Get viewport (the main window area)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(
        ImVec2(viewport->Pos.x, viewport->Pos.y + GlobalSettingsWindow::WINDOW_HEIGHT),
        ImGuiCond_Always
    );

    const float windowWidth = viewport->Size.x - Spectrogram::WINDOW_WIDTH - LevelsDisplay::WINDOW_WIDTH;
    ImGui::SetNextWindowSize(ImVec2(windowWidth, WINDOW_HEIGHT), ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin("Preset loader", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse);
}

void PresetLoaderWindow::DrawPresetLoader(AudioPreset& preset) const {
    ImGui::SeparatorText("Preset loader");

    // --- Browse and export preset file ---
    if (ImGui::Button("Export Preset")) {
        IGFD::FileDialogConfig config;
        config.path = "."; // starting directory
        config.countSelectionMax = 1;
        ImGuiFileDialog::Instance()->OpenDialog("SavePresetDlg", "Export Preset", ".json", config);
    }

    if (ImGuiFileDialog::Instance()->Display("SavePresetDlg")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            if (AudioPresetIO::SaveToFile(preset, filePath))
                ImGui::OpenPopup("SaveSuccess");
            else
                ImGui::OpenPopup("SaveFail");
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGui::BeginPopup("SaveSuccess")) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Preset exported successfully!");
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("SaveFail")) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Failed to export preset!");
        ImGui::EndPopup();
    }

    ImGui::SameLine();

    // --- Browse and load preset file  ---
    if (ImGui::Button("Load Preset")) {
        IGFD::FileDialogConfig config;
        config.path = "."; // starting directory
        config.countSelectionMax = 1;
        ImGuiFileDialog::Instance()->OpenDialog("LoadPresetDlg", "Load Preset", ".json", config);
    }

    if (ImGuiFileDialog::Instance()->Display("LoadPresetDlg")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            if (AudioPresetIO::LoadFromFile(preset, filePath))
                ImGui::OpenPopup("LoadSuccess");
            else
                ImGui::OpenPopup("LoadFail");
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGui::BeginPopup("LoadSuccess")) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Preset loaded successfully!");
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("LoadFail")) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Failed to load preset!");
        ImGui::EndPopup();
    }


    // --- Select and load built-in preset ---
    auto& loader = BuiltInPresetsLoader::GetShared();
    auto& presetNames = loader.GetPresetNames();
    static int currentPresetIndex = loader.GetIndexOfDefaultPreset(); // index of the selected preset

    if (presetNames.empty()) {
        ImGui::TextDisabled("No built-in presets found.");
        return;
    }

    // Current label: show the selected preset name, or placeholder
    const char* currentLabel =
        (currentPresetIndex >= 0 && currentPresetIndex < (int)presetNames.size())
        ? presetNames[currentPresetIndex].c_str()
        : "Select preset...";

    if (ImGui::BeginCombo("Built-in Presets", currentLabel)) {
        for (int i = 0; i < (int)presetNames.size(); i++) {
            bool isSelected = (currentPresetIndex == i);
            if (ImGui::Selectable(presetNames[i].c_str(), isSelected)) {
                currentPresetIndex = i;

                // Load the selected preset
                loader.LoadBuiltInPreset(preset, presetNames[i]);

                ImGui::OpenPopup("PresetLoadedPopup");
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Optional: feedback popup
    if (ImGui::BeginPopup("PresetLoadedPopup")) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f),
                           "Preset '%s' loaded successfully!",
                           presetNames[currentPresetIndex].c_str());
        ImGui::EndPopup();
    }
}