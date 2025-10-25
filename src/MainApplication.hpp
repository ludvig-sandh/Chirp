// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

// Important to include GUIManager first since it includes all imgui-related headers.
// They need to be included first in order not to cause any conflicts with preferred integral types.
#include "gui/GUIManager.hpp"
#include "engine/AudioEngine.hpp"
#include "preset/AudioPreset.hpp"
#include "fft/FFTComputer.hpp"

#include <memory>
#include <iostream>
#include <thread>
#include <atomic>

class MainApplication {
public:
    MainApplication();

    void Start();

private:
    std::shared_ptr<AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    GUIManager m_gui;
    AudioEngine m_audioEngine;

    std::atomic<bool> m_isRunning = true;
};