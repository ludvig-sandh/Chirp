// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/windows/KeyboardWindow.hpp"

void UIKey::Draw(const ImVec2& offset, ImDrawList* drawList, bool isPressed) const {
    static const ImU32 pressedKeyColor = IM_COL32(
        GUIConstants::Colors::HIGHLIGHT[0],
        GUIConstants::Colors::HIGHLIGHT[1],
        GUIConstants::Colors::HIGHLIGHT[2],
        GUIConstants::Colors::HIGHLIGHT[3]
    );
    ImU32 color = isPressed
        ? pressedKeyColor
        : (note.IsBlackKey() ? BLACK_KEY_COLOR : WHITE_KEY_COLOR);

    // Applying the cursor offset (computing absolute window positions)
    ImVec2 rectMin = ImVec2(rect.Min.x + offset.x, rect.Min.y + offset.y);
    ImVec2 rectMax = ImVec2(rect.Max.x + offset.x, rect.Max.y + offset.y);

    drawList->AddRectFilled(rectMin, rectMax, color, 2.0f);
    drawList->AddRect(rectMin, rectMax, IM_COL32(0, 0, 0, 255), 2.0f);
}

KeyboardWindow::KeyboardWindow(int keyboardWidth)
    : m_numKeys(std::abs(FIRST_NOTE - LAST_NOTE) + 1) // Inclusive ends
    , m_keys(HelpCreateKeys(keyboardWidth))
    , m_windowWidth(keyboardWidth)
{}

std::set<Audio::Core::Note> KeyboardWindow::Render(const std::set<Audio::Core::Note>& pressedQwertyNotes) const {
    ConfigureWindow();

    const ImVec2 offset = ImGui::GetCursorScreenPos();
    const ImVec2 mouseAbs = ImGui::GetIO().MousePos;
    const ImVec2 mouseRel = ImVec2(mouseAbs.x - offset.x, mouseAbs.y - offset.y);

    // Collect currently pressed notes (from keyboard/mouse)
    std::set<Audio::Core::Note> pressedNotes = pressedQwertyNotes;
    std::optional<Audio::Core::Note> notePressedByMouse = GetMouseKeyboardInput(mouseRel);
    if (notePressedByMouse.has_value()) {
        pressedNotes.insert(*notePressedByMouse);
    }

    DrawAllKeys(offset, pressedNotes);

    ImGui::Dummy(ImVec2(static_cast<float>(m_windowWidth), WHITE_KEY_HEIGHT));
    ImGui::End();

    return pressedNotes;
}


void KeyboardWindow::ConfigureWindow() const {
    // Get viewport (the main window area)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Force window to bottom, full width
    ImGui::SetNextWindowPos(
        ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - WINDOW_HEIGHT),
        ImGuiCond_Always
    );
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, WINDOW_HEIGHT), ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin("Piano", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::SeparatorText("Piano");
}

std::vector<UIKey> KeyboardWindow::HelpCreateKeys(int keyboardWidth) {
    std::vector<UIKey> result;
    result.reserve(std::abs(FIRST_NOTE - LAST_NOTE));

    int numWhiteKeys = HelpCountWhiteKeys();
    float whiteKeyWidth = static_cast<float>(keyboardWidth - KEYBOARD_PADDING) / numWhiteKeys;
    float blackKeyWidth = whiteKeyWidth / WHITE_TO_BLACK_KEY_WIDTH_RATIO;
    float x = 0.0f;
    for (Audio::Core::Note note = FIRST_NOTE; note <= LAST_NOTE; ++note) {
        ImVec2 keyMin;
        ImVec2 keyMax;
        if (note.IsBlackKey()) {
            keyMin = ImVec2(x - blackKeyWidth / 2, 0.0f);
            keyMax = ImVec2(x + blackKeyWidth / 2, BLACK_KEY_HEIGHT);
        }else {
            keyMin = ImVec2(x, 0.0f);
            keyMax = ImVec2(x + whiteKeyWidth, WHITE_KEY_HEIGHT);
            x += whiteKeyWidth;
        }
        result.push_back(UIKey{note, ImRect(keyMin, keyMax)});
    }
    return result;
}

int KeyboardWindow::HelpCountWhiteKeys() {
    int count = 0;
    for (Audio::Core::Note note = FIRST_NOTE; note <= LAST_NOTE; ++note) {
        if (!note.IsBlackKey()) {
            count++;
        }
    }
    return count;
}

std::optional<Audio::Core::Note> KeyboardWindow::GetMouseKeyboardInput(const ImVec2& mousePosRelative) const {
    bool isMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    auto findInFiltered = [&](auto filter) -> std::optional<Audio::Core::Note> {
        for (auto const& key : m_keys | std::views::filter(filter)) {
            if (key.rect.Contains(mousePosRelative) && isMouseDown)
                return key.note;
        }
        return std::nullopt;
    };

    // Black first (topmost keys)
    if (auto note = findInFiltered(BLACK_FILTER); note.has_value()) {
        return note;
    }

    // Then white keys
    return findInFiltered(WHITE_FILTER);
}

void KeyboardWindow::DrawAllKeys(const ImVec2& offset, std::set<Audio::Core::Note> pressedNotes) const {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    auto drawFilteredKeys = [&](auto&& filter) {
        for (auto const& key : m_keys | std::views::filter(filter)) {
            bool isPressed = pressedNotes.find(key.note) != pressedNotes.end();
            key.Draw(offset, drawList, isPressed);
        }
    };

    // Draw all white keys before black keys
    drawFilteredKeys(WHITE_FILTER);
    drawFilteredKeys(BLACK_FILTER);
}