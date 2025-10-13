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

#include <memory>
#include <iostream>
#include <format>

// RAII class for managing the GLFW window
class GUIManager {
public:
    GUIManager(std::shared_ptr<AudioPreset> preset);

    ~GUIManager();

    void RunMainLoop();
    
    static void glfw_error_callback(int error, const char* description);
    
private:
    GLFWwindow *InitAux();

    void DeinitAux();

    std::shared_ptr<AudioPreset> m_preset;
    GLFWwindow *m_window;
    ImGuiIO *m_io;

    // Background color
    static inline ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};