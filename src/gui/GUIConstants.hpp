// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "ImGuiDefinitions.hpp"
#include "imgui.h"
#include <array>

namespace GUIConstants {
    namespace Colors {
        // Used more for the images on screen
        inline constexpr std::array<unsigned char, 3> BG{5, 9, 19};
        inline constexpr std::array<unsigned char, 3> MIDTONE{31, 64, 132};
        inline constexpr std::array<unsigned char, 3> HIGHLIGHT{107, 201, 255};

        // Used more for control elements such as sliders and stuff
        inline constexpr std::array<unsigned char, 3> TERTIARY{31, 64, 107};
        inline constexpr std::array<unsigned char, 3> SECONDARY{45, 98, 165};
        inline constexpr std::array<unsigned char, 3> PRIMARY{61, 133, 224};

        // Converts an array of RGB bytes (0-255) to an ImVec type (0-1) 
        inline ImVec4 ToImVec(const std::array<unsigned char, 3>& color) {
            return ImVec4(
                color[0] / 255.0f,
                color[1] / 255.0f,
                color[2] / 255.0f,
                1.0f
            );
        }
    }
}