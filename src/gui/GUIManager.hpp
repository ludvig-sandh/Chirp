#pragma once


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "AudioPreset.hpp"
#include "Spectrogram.hpp"
#include "FFTComputer.hpp"
#include "LevelsDisplay.hpp"

#include <memory>
#include <iostream>
#include <format>

// RAII class for managing the GLFW window
class GUIManager {
public:
    GUIManager(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer);

    ~GUIManager();

    void RunMainLoop();
    
    static void glfw_error_callback(int error, const char* description);
    
private:
    void DrawChirpUI();
    void DrawSynthUI();

    GLFWwindow *InitAux();
    void DeinitAux();

    void HandleKeyboardInput();

    std::shared_ptr<AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;

    GLFWwindow *m_window;
    ImGuiIO *m_io;
    Spectrogram m_spectrogram;
    LevelsDisplay m_levelsDisplay;

    // Background color
    static inline ImVec4 clear_color = ImVec4(17.0 / 255.0, 38.0 / 255.0, 92.0 / 255.0, 0.5f);
};