// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "ImGuiDefinitions.hpp"
#include "imgui.h"
#include "GUIConstants.hpp"

// RAII-type class that pushes a color theme and pops it after the object dies.
class ColorThemeApplier {
public:
    ColorThemeApplier() {
        // Push temporary widget colors
        static const ImVec4 primary = GUIConstants::Colors::ToImVec(GUIConstants::Colors::PRIMARY);
        static const ImVec4 secondary = GUIConstants::Colors::ToImVec(GUIConstants::Colors::SECONDARY);
        static const ImVec4 tertiary = GUIConstants::Colors::ToImVec(GUIConstants::Colors::TERTIARY);
        static const ImVec4 highlight = GUIConstants::Colors::ToImVec(GUIConstants::Colors::HIGHLIGHT);
        SetColor(ImGuiCol_FrameBg, tertiary);
        SetColor(ImGuiCol_FrameBgHovered, secondary);
        SetColor(ImGuiCol_FrameBgActive, secondary);
        SetColor(ImGuiCol_Button, tertiary);
        SetColor(ImGuiCol_ButtonHovered, secondary);
        SetColor(ImGuiCol_ButtonActive, primary);
        SetColor(ImGuiCol_CheckMark, highlight);
        SetColor(ImGuiCol_SliderGrab, primary);
        SetColor(ImGuiCol_SliderGrabActive, highlight);
    }

    // Pops the exact number of pushed colors.
    ~ColorThemeApplier() {
        ImGui::PopStyleColor(m_pushCount);
    }

private:
    void SetColor(ImGuiCol idx, const ImVec4& col) {
        ImGui::PushStyleColor(idx, col);
        m_pushCount++;
    }

    int m_pushCount = 0;
};