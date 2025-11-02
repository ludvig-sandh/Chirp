#pragma once

#include "imgui.h"
#include "imgui_internal.h" // For ImRect
#include "audio/core/Frequency.hpp"
#include <vector>
#include <ranges>
#include <optional>
#include <set>

struct UIKey {
    Note note; // Corresponding note
    ImRect rect; // Relative coordinates to top left of keyboard

    // Adds the key to the drawlist
    void Draw(const ImVec2& offset, ImDrawList* drawList, bool isPressed) const {
        ImU32 color = isPressed
            ? PRESSED_KEY_COLOR
            : (note.IsBlackKey() ? BLACK_KEY_COLOR : WHITE_KEY_COLOR);

        // Applying the cursor offset (computing absolute window positions)
        ImVec2 rectMin = ImVec2(rect.Min.x + offset.x, rect.Min.y + offset.y);
        ImVec2 rectMax = ImVec2(rect.Max.x + offset.x, rect.Max.y + offset.y);

        drawList->AddRectFilled(rectMin, rectMax, color, 2.0f);
        drawList->AddRect(rectMin, rectMax, IM_COL32(0, 0, 0, 255), 2.0f);
    }

    static inline const ImU32 BLACK_KEY_COLOR = IM_COL32(20, 20, 20, 255);
    static inline const ImU32 WHITE_KEY_COLOR = IM_COL32(240, 240, 240, 255);
    static inline const ImU32 PRESSED_KEY_COLOR = IM_COL32(107, 201, 255, 255);
};

// Represents the keyboard UI element
class Keyboard {
public:
    // First and last key of keyboard
    static inline const Note FIRST_NOTE{Key::A, 0};
    static inline const Note LAST_NOTE{Key::C, 8};

    Keyboard(size_t keyboardWidth);

    // Renders the keyboard and all pressed notes. The set of pressed notes
    // from QWERTY keyboard (rather than midi keyboard) is provided via parameter "pressedQwertyNotes".
    // Returns all pressed notes (pressedQwertyNotes + possible extra due to mouse press)
    std::set<Note> Render(const std::set<Note>& pressedQwertyNotes) const;

private:
    void ConfigureWindow() const;

    static std::vector<UIKey> HelpCreateKeys(size_t keyboardWidth);

    // Counts the number of white keys between 
    static size_t HelpCountWhiteKeys();

    // Returns a pointer to the key in m_keys that is currently pressed via mouse click, or if none is pressed, nullptr
    std::optional<Note> GetMouseKeyboardInput(const ImVec2& mousePosRelative) const;

    // Draws the keys one by one, making sure any pressed key is highlighted
    void DrawAllKeys(const ImVec2& offset, std::set<Note> pressedNotes) const;

    size_t m_numKeys;
    std::vector<UIKey> m_keys;
    size_t m_keyboardWidth;

    static inline const float KEYBOARD_HEIGHT = 160.0f;
    static inline const float WHITE_KEY_HEIGHT = 120.0f;
    static inline const float BLACK_KEY_HEIGHT = 70.0f;
    static inline const float WHITE_TO_BLACK_KEY_WIDTH_RATIO = 40.0f / 24.0f;

    // Filter lambdas
    static inline const auto BLACK_FILTER = [](UIKey const& k){ return k.note.IsBlackKey(); };
    static inline const auto WHITE_FILTER = [](UIKey const& k){ return !k.note.IsBlackKey(); };
};
