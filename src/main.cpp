// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

// Important to include GUIManager first since it includes all imgui-related headers.
// They need to be included first in order not to cause any conflicts with preferred integral types.
#include "GUIManager.hpp"
#include "AudioEngine.hpp"
#include "AudioPreset.hpp"
#include "FFTComputer.hpp"

#include <memory>
#include <iostream>
#include <thread>
#include <atomic>

class MainApplication {
public:
    MainApplication()
        : m_preset(std::make_shared<AudioPreset>())
        , m_fftComputer(std::make_shared<FFTComputer>())
        , m_gui(m_preset, m_fftComputer)
        , m_audioEngine(m_preset, m_fftComputer)
    {}

    void Start() {
        // Start the mixer in its own thread
        std::thread audioThread([&]() {
            m_audioEngine.Start(m_isRunning);
        });

        // Start the FFT computer in its own thread
        std::thread fftThread([&]() {
            m_fftComputer->Start(m_isRunning);
        });

        // Start gui on main thread, blocks until application closes
        m_gui.RunMainLoop();

        // Signal the audio engine to stop
        m_isRunning.store(false);
        audioThread.join();
        fftThread.join();
    }

private:
    std::shared_ptr<AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    GUIManager m_gui;
    AudioEngine m_audioEngine;

    std::atomic<bool> m_isRunning = true;
};

int main(int, char**)
{
    MainApplication app;
    app.Start();
    return 0;
}
