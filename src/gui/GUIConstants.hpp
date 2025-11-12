// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "gui/ImGuiDefinitions.hpp"
#include "imgui.h"
#include <array>

// Color theme etc.
namespace GUIConstants::Colors {
    // Used more for the images on screen
    inline constexpr std::array<unsigned char, 4> MIDTONE{31, 64, 132, 255};
    inline constexpr std::array<unsigned char, 4> HIGHLIGHT{107, 201, 255, 255};

    // Used more for control elements such as sliders and stuff
    inline constexpr std::array<unsigned char, 4> TERTIARY{31, 64, 107, 255};
    inline constexpr std::array<unsigned char, 4> SECONDARY{45, 98, 165, 255};
    inline constexpr std::array<unsigned char, 4> PRIMARY{61, 133, 224, 255};
    
    // Background color
    inline constexpr std::array<unsigned char, 4> BG{17, 38, 92, 127};

    // Converts an array of RGB bytes (0-255) to an ImVec type (0-1) 
    inline ImVec4 ToImVec(const std::array<unsigned char, 4>& color) {
        return ImVec4(
            color[0] / 255.0f,
            color[1] / 255.0f,
            color[2] / 255.0f,
            color[3] / 255.0f
        );
    }
}