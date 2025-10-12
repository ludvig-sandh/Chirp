// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

// Important to include GUIManager first since it includes all imgui-related headers.
// They need to be included first in order not to cause any conflicts with preferred integral types.
#include "GUIManager.cpp"
#include "AudioEngine.hpp"
#include "AudioPreset.hpp"

#include <memory>
#include <iostream>
#include <thread>
#include <atomic>

class MainApplication {
public:
    MainApplication()
        : m_preset(std::make_shared<AudioPreset>())
        , m_gui(m_preset)
        , m_audioEngine(m_preset)
    {}

    void RunMainLoop() {
        // Start the mixer in its own thread
        std::thread audioThread([&]() {
            m_audioEngine.Start(m_isRunning);
        });

        while (!glfwWindowShouldClose(m_gui.GetWindow()))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();
            if (glfwGetWindowAttrib(m_gui.GetWindow(), GLFW_ICONIFIED) != 0)
            {
                ImGui_ImplGlfw_Sleep(10);
                continue;
            }

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
            {
                static int counter = 0;

                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

                float volumeTemp = m_preset->volume.load();
                ImGui::SliderFloat("Volume", &volumeTemp, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                m_preset->volume.store(volumeTemp);

                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                float framerate = m_gui.GetIO()->Framerate;
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(m_gui.GetWindow(), &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(m_gui.GetWindow());
        }

        m_isRunning.store(false);
        audioThread.join();
    }
private:
    std::shared_ptr<AudioPreset> m_preset;
    GUIManager m_gui;
    AudioEngine m_audioEngine;
    std::atomic<bool> m_isRunning = true;
};

int main(int, char**)
{
    MainApplication app;
    app.RunMainLoop();
    return 0;
}
