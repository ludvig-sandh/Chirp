// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "ImGuiDefinitions.hpp"
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "audio/preset/AudioPreset.hpp"

class PresetLoaderWindow {
public:
    static inline const int WINDOW_HEIGHT = 84;

    void Render(AudioPreset& preset) const;

private:
    void ConfigureWindow() const;

    // Display export/load preset buttons
    void DrawPresetLoader(AudioPreset& preset) const;
};