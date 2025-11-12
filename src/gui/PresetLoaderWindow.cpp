// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/PresetLoaderWindow.hpp"

#include "gui/Spectrogram.hpp"
#include "gui/LevelsWindow.hpp"
#include "gui/GlobalSettingsWindow.hpp"
#include "audio/preset/AudioPresetSerialization.hpp"
#include "audio/preset/BuiltInPresetsLoader.hpp"

#include <optional>
#include <cassert>

void PresetLoaderWindow::Render(Audio::Preset::AudioPreset& preset) const {
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

    const float windowWidth = viewport->Size.x - Spectrogram::WINDOW_WIDTH - LevelsWindow::WINDOW_WIDTH;
    ImGui::SetNextWindowSize(ImVec2(windowWidth, WINDOW_HEIGHT), ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin("audio/preset loader", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse);
}

void PresetLoaderWindow::DrawPresetLoader(Audio::Preset::AudioPreset& preset) const {
    ImGui::SeparatorText("audio/preset loader");

    // --- Browse and export preset file ---
    if (ImGui::Button("Export Preset")) {
        IGFD::FileDialogConfig config;
        config.path = "."; // starting directory
        config.countSelectionMax = 1;
        ImGuiFileDialog::Instance()->OpenDialog("SavePresetDlg", "Export Preset", ".json", config);
    }

    if (ImGuiFileDialog::Instance()->Display(
        "SavePresetDlg",
        ImGuiWindowFlags_NoCollapse,
        ImVec2(800, 400),
        ImVec2(1100, 700)
    )) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            if (Audio::Preset::IO::SaveToFile(preset, filePath))
                ImGui::OpenPopup("SaveSuccess");
            else
                ImGui::OpenPopup("SaveFail");
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGui::BeginPopup("SaveSuccess")) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "audio/preset exported successfully!");
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

    if (ImGuiFileDialog::Instance()->Display(
        "LoadPresetDlg",
        ImGuiWindowFlags_NoCollapse,
        ImVec2(800, 400),
        ImVec2(1100, 700)
    )) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            if (Audio::Preset::IO::LoadFromFile(preset, filePath))
                ImGui::OpenPopup("LoadSuccess");
            else
                ImGui::OpenPopup("LoadFail");
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGui::BeginPopup("LoadSuccess")) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "audio/preset loaded successfully!");
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("LoadFail")) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Failed to load preset!");
        ImGui::EndPopup();
    }


    // --- Select and load built-in preset ---
    auto& loader = Audio::Preset::BuiltInPresetsLoader::GetShared();
    auto& presetNames = loader.GetPresetNames();
    
    if (presetNames.empty()) {
        ImGui::TextDisabled("No built-in presets found.");
        return;
    }

    static int selectedPresetIndex = loader.GetIndexOfDefaultPreset().value_or(0);
    assert(selectedPresetIndex < std::ssize(presetNames) && "PROGRAMMING ERROR: GetIndexOfDefaultPreset() returned index larger than list of presets itself.");

    // Current label: show the selected preset name, or placeholder
    const char* currentLabel = presetNames[selectedPresetIndex].c_str();

    if (ImGui::BeginCombo("Built-in Presets", currentLabel)) {
        for (int i = 0; i < std::ssize(presetNames); i++) {
            bool isSelected = (selectedPresetIndex == i);
            if (ImGui::Selectable(presetNames[i].c_str(), isSelected)) {
                selectedPresetIndex = i;

                // Load the selected preset
                loader.LoadBuiltInPreset(preset, presetNames[i]);

                ImGui::OpenPopup("audio/presetLoadedPopup");
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Optional: feedback popup
    if (ImGui::BeginPopup("audio/presetLoadedPopup")) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f),
                           "audio/preset '%s' loaded successfully!",
                           presetNames[selectedPresetIndex].c_str());
        ImGui::EndPopup();
    }
}