// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "gui/ImGuiDefinitions.hpp"
#include "imgui.h"
#include "audio/preset/AudioPreset.hpp"
#include <memory>

class PresetWindow {
public:
    PresetWindow(std::shared_ptr<Audio::Preset::AudioPreset> preset)
        : m_preset(preset)
    {}

    void Render(float framerate) const;

private:
    void ConfigureWindow() const;

    void DrawOscillatorA() const;
    void DrawOscillatorB() const;
    void DrawVolumeEnvelope() const;
    void DrawLowPassFilter() const;
    void DrawHighPassFilter() const;
    void DrawFeedbackDelay() const;
    void DrawReverb() const;

    // Display controls related to LFOs
    void DrawLFO1() const;
    void DrawLFO2() const;

    std::shared_ptr<Audio::Preset::AudioPreset> m_preset;
};