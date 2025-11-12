// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "gui/ImGuiDefinitions.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h" // For ImRect
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#include "audio/preset/AudioPreset.hpp"
#include "gui/windows/SpectrogramWindow.hpp"
#include "fft/FFTComputer.hpp"
#include "gui/windows/LevelsWindow.hpp"
#include "gui/windows/KeyboardWindow.hpp"
#include "gui/windows/PresetWindow.hpp"
#include "gui/windows/GlobalSettingsWindow.hpp"
#include "gui/windows/PresetLoaderWindow.hpp"
#include "gui/windows/WaveformWindow.hpp"
#include "gui/windows/SpectrumWindow.hpp"
#include "midi/MidiInput.hpp"

#include <memory>
#include <set>

// Responsible for managing the GLFW window and running the main loop
class GUIManager {
public:
    // Constructs the GUI manager with the given audio preset and FFT processor
    GUIManager(std::shared_ptr<Audio::Preset::AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer);

    // Cleans up GUI resources and shuts down the window system
    ~GUIManager();

    // Runs the main application loop, processing input and rendering all windows
    void Run();
    
    // GLFW error callback for reporting initialization or runtime errors
    static void GFLWErrorCallback(int error, const char* description);
    
private:
    // Initializes GLFW, ImGui, and window resources; returns the created window handle
    GLFWwindow *InitAux();
    
    // Releases GLFW, ImGui, and related GUI resources
    void DeinitAux();

    // Returns a set of notes currently pressed using the keyboard (not midi keyboard, QWERTY keyboard)
    std::set<Audio::Core::Note> GetQwertyNotesPressed() const;

    // Returns a set of all notes pressed (QWERTY + MIDI input)
    std::set<Audio::Core::Note> GetAllPressedNotes();

    std::shared_ptr<Audio::Preset::AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    KeyboardWindow m_keyboard;
    PresetWindow m_presetWindow;
    WaveformWindow m_oscAWaveformWindow;
    WaveformWindow m_oscBWaveformWindow;
    SpectrumWindow m_spectrumWindow;

    GLFWwindow *m_window;
    ImGuiIO *m_io;
    SpectrogramWindow m_spectrogram;
    LevelsWindow m_levelsDisplay;
    GlobalSettingsWindow m_globalSettingsWindow;
    PresetLoaderWindow m_presetLoaderWindow;
    MidiInput m_midiInput;

    static inline const int SCREEN_WIDTH = 1280;
    static inline const int SCREEN_HEIGHT = 800;
}; 