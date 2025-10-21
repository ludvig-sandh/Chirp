// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "MainApplication.hpp"

MainApplication::MainApplication()
    : m_preset(std::make_shared<AudioPreset>())
    , m_fftComputer(std::make_shared<FFTComputer>())
    , m_gui(m_preset, m_fftComputer)
    , m_audioEngine(m_preset, m_fftComputer)
{}

void MainApplication::Start() {
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