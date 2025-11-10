// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "ImGuiDefinitions.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h" // For ImRect
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#include "audio/preset/AudioPreset.hpp"
#include "gui/Spectrogram.hpp"
#include "fft/FFTComputer.hpp"
#include "gui/LevelsDisplay.hpp"
#include "gui/Keyboard.hpp"
#include "gui/PresetWindow.hpp"
#include "gui/GlobalSettingsWindow.hpp"
#include "gui/PresetLoaderWindow.hpp"
#include "gui/WaveformWindow.hpp"
#include "gui/SpectrumWindow.hpp"
#include "midi/MidiInput.hpp"

#include <memory>
#include <set>

// Responsible for managing the GLFW window and running the main loop
class GUIManager {
public:
    GUIManager(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer);

    ~GUIManager();

    void RunMainLoop();
    
    static void glfw_error_callback(int error, const char* description);
    
private:
    GLFWwindow *InitAux();
    void DeinitAux();

    std::set<Note> GetQwertyNotesPressed() const;

    std::set<Note> GetAllPressedNotes();

    std::shared_ptr<AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    Keyboard m_keyboard;
    PresetWindow m_presetWindow;
    WaveformWindow m_oscAWaveformWindow;
    WaveformWindow m_oscBWaveformWindow;
    SpectrumWindow m_spectrumWindow;

    GLFWwindow *m_window;
    ImGuiIO *m_io;
    Spectrogram m_spectrogram;
    LevelsDisplay m_levelsDisplay;
    GlobalSettingsWindow m_globalSettingsWindow;
    PresetLoaderWindow m_presetLoaderWindow;
    MidiInput m_midiInput;

    // Background color
    static inline const ImVec4 CLEAR_COLOR = ImVec4(17.0 / 255.0, 38.0 / 255.0, 92.0 / 255.0, 0.5f);

    static inline const size_t SCREEN_WIDTH = 1280;
    static inline const size_t SCREEN_HEIGHT = 800;
}; 