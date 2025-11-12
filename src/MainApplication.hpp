// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

// Important to include GUIManager first since it includes all imgui-related headers.
// They need to be included first in order not to cause any conflicts with preferred integral types.
#include "gui/GUIManager.hpp"
#include "audio/engine/AudioEngine.hpp"
#include "audio/preset/AudioPreset.hpp"
#include "fft/FFTComputer.hpp"

#include <memory>
#include <thread>
#include <atomic>

class MainApplication {
public:
    MainApplication();

    void Start();

private:
    std::shared_ptr<Audio::Preset::AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    GUI::GUIManager m_gui;
    Audio::Engine::AudioEngine m_audioEngine;

    std::atomic<bool> m_isRunning = true;
};