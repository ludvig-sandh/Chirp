// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "ImGuiDefinitions.hpp"
#include "imgui.h"
#include "imgui_internal.h" // For ImRect
#include "gui/GUIConstants.hpp"
#include "audio/core/Frequency.hpp"
#include <vector>
#include <ranges>
#include <optional>
#include <set>

struct UIKey {
    Audio::Core::Note note; // Corresponding note
    ImRect rect; // Relative coordinates to top left of keyboard

    // Adds the key to the drawlist
    void Draw(const ImVec2& offset, ImDrawList* drawList, bool isPressed) const;

    static inline const ImU32 BLACK_KEY_COLOR = IM_COL32(20, 20, 20, 255);
    static inline const ImU32 WHITE_KEY_COLOR = IM_COL32(240, 240, 240, 255);
};

// Represents the keyboard UI element
class KeyboardWindow {
public:
    static inline const float WINDOW_HEIGHT = 160.0f;
    
    // First and last key of keyboard
    static inline const Audio::Core::Note FIRST_NOTE{Audio::Core::Key::A, 0};
    static inline const Audio::Core::Note LAST_NOTE{Audio::Core::Key::C, 8};

    KeyboardWindow(int windowWidth);

    // Renders the keyboard and all pressed notes. The set of pressed notes
    // from QWERTY keyboard (rather than midi keyboard) is provided via parameter "pressedQwertyNotes".
    // Returns all pressed notes (pressedQwertyNotes + possible extra due to mouse press)
    std::set<Audio::Core::Note> Render(const std::set<Audio::Core::Note>& pressedQwertyNotes) const;

private:
    void ConfigureWindow() const;

    static std::vector<UIKey> HelpCreateKeys(int keyboardWidth);

    // Counts the number of white keys between 
    static int HelpCountWhiteKeys();

    // Returns a pointer to the key in m_keys that is currently pressed via mouse click, or if none is pressed, nullptr
    std::optional<Audio::Core::Note> GetMouseKeyboardInput(const ImVec2& mousePosRelative) const;

    // Draws the keys one by one, making sure any pressed key is highlighted
    void DrawAllKeys(const ImVec2& offset, std::set<Audio::Core::Note> pressedNotes) const;

    int m_numKeys;
    std::vector<UIKey> m_keys;
    int m_windowWidth;

    static inline const float WHITE_KEY_HEIGHT = 120.0f;
    static inline const float BLACK_KEY_HEIGHT = 70.0f;
    static inline const float WHITE_TO_BLACK_KEY_WIDTH_RATIO = 40.0f / 24.0f;

    // Padding after last key before right window border
    static inline const int KEYBOARD_PADDING = 16;

    // Filter lambdas
    static inline const auto BLACK_FILTER = [](UIKey const& k){ return k.note.IsBlackKey(); };
    static inline const auto WHITE_FILTER = [](UIKey const& k){ return !k.note.IsBlackKey(); };
};
