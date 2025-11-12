// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "gui/ImGuiDefinitions.hpp"
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "audio/preset/AudioPreset.hpp"

namespace GUI::Window {

class GlobalSettingsWindow {
public:
    static inline const int WINDOW_HEIGHT = 60;

    void Render(Audio::Preset::AudioPreset& preset) const;

private:
    void ConfigureWindow() const;
    void DrawGlobalSettings(Audio::Preset::AudioPreset& preset) const;
};

} // namespace GUI::Window
