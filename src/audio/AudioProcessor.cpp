// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "AudioProcessor.hpp"
#include "AudioEngine.hpp"
#include "AudioBackend.hpp"
#include "LFO.hpp"
#include <iostream>

void AudioProcessor::AddChild(std::shared_ptr<AudioProcessor> child) {
    m_children.insert(child);
}

void AudioProcessor::ClearModulations() {
    ClearModulationsImpl(); // Clears for this node
    for (const auto& child : m_children) { // Clears all children nodes
        child->ClearModulations();
    }
}

void AudioProcessor::ApplyModulation(float amount, ModulationType modType) {
    std::cerr << "WARNING: Tried to apply modulation on a node that doesn't support it.\n";
}

AudioFrame AudioProcessor::GenerateFrame(const AudioPreset& preset) {
    if (m_visited) {
        // Don't compute the result of this node twice
        return m_cachedResult;
    }
    AudioFrame frame;

    // Process all children first
    for (const auto& child : m_children) {
        frame += child->GenerateFrame(preset);
    }

    AudioFrame bypassedFrame(frame);
    AudioFrame processedFrame(frame);

    if (isOn) {
        ProcessFrame(processedFrame);
        ApplyGainAndPan(processedFrame);
        frame = AudioFrame::Blend(processedFrame, bypassedFrame, mix);
    }else {
        frame = bypassedFrame;
    }

    frame.ClipToValidRange();

    m_visited = true;
    m_cachedResult = frame;
    return m_cachedResult;
}

void AudioProcessor::ClearVisited() {
    m_visited = false;
    for (const std::shared_ptr<AudioProcessor>& child : m_children) {
        child->ClearVisited();
    }
}

void AudioProcessor::ApplyGainAndPan(AudioFrame& output) {
    output = gain.Apply(output);
    output = pan.Apply(output);
}
