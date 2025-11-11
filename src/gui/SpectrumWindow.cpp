// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/SpectrumWindow.hpp"
#include "ImGuiDefinitions.hpp"
#include "imgui.h"
#include "gui/LevelsDisplay.hpp"
#include "gui/Keyboard.hpp"
#include "gui/Spectrogram.hpp"
#include "gui/GUIConstants.hpp"
#include <algorithm>

void SpectrumWindow::PushMagnitudes(const std::vector<float>& magnitudes) {
    if (std::ssize(m_magnitudes) != std::ssize(magnitudes)) {
        m_magnitudes = magnitudes;
        m_smoothed.assign(std::ssize(magnitudes), 0.0f);
    }

    // Smoothing so bars don't jitter too hard
    for (int i = 0; i < std::ssize(magnitudes); i++) {
        float clamped = std::clamp(magnitudes[i], 0.0f, 1.0f);
        m_smoothed[i] = SMOOTHING_FACTOR * m_smoothed[i] + (1.0f - SMOOTHING_FACTOR) * clamped;
    }
}

void SpectrumWindow::Render() {
    if (m_smoothed.empty()) return;

    ConfigureWindow();

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const int n = std::ssize(m_smoothed);
    float barWidth = size.x / (float)n;

    for (int i = 0; i < n; i++) {
        float mag = m_smoothed[i];
        float height = mag * size.y;
        
        // Larger magnitude => lighter color. Remapped magnitude to a lighter color curve:
        float m = 1.0f - (1.0f - mag) * (1.0f - mag);
        m = std::min(m * 2, 1.0f);
        ImU32 color = IM_COL32(
            GUIConstants::Colors::HIGHLIGHT[0] * m + GUIConstants::Colors::MIDTONE[0] * (1.0f - m),
            GUIConstants::Colors::HIGHLIGHT[1] * m + GUIConstants::Colors::MIDTONE[1] * (1.0f - m),
            GUIConstants::Colors::HIGHLIGHT[2] * m + GUIConstants::Colors::MIDTONE[2] * (1.0f - m),
            GUIConstants::Colors::HIGHLIGHT[3] * m + GUIConstants::Colors::MIDTONE[3] * (1.0f - m)
        );

        ImVec2 p0(pos.x + i * barWidth, pos.y + size.y);
        ImVec2 p1(pos.x + (i + 1) * barWidth, pos.y + size.y - height);

        draw_list->AddRectFilled(p1, p0, color);
    }

    // Ensure cursor is placed after drawn area
    ImGui::Dummy(size);

    ImGui::End();
}

void SpectrumWindow::ConfigureWindow() const {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    const float x = viewport->Pos.x + viewport->Size.x - Spectrogram::WINDOW_WIDTH - LevelsDisplay::WINDOW_WIDTH;
    const float h = (viewport->Size.y - Spectrogram::WINDOW_HEIGHT - Keyboard::WINDOW_HEIGHT) / 2.0f;
    const float y = viewport->Pos.y + Spectrogram::WINDOW_HEIGHT + h;
    const float w = (viewport->Size.x - x);

    ImGui::SetNextWindowPos(
        ImVec2(x, y),  // stack under spectrogram
        ImGuiCond_Always
    );
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);

    ImGui::Begin("Spectrum", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse
    );

    ImGui::SeparatorText("Spectrum");
}
