// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <unordered_set>

#include "audio/core/Gain.hpp"
#include "audio/core/Pan.hpp"
#include "audio/modulation/ModulationMatrix.hpp"
#include "util/NormalizedFloat.hpp"

namespace Audio::Engine {

// Represents a node in the audio graph showing how audio is routed throughout the engine.
class AudioProcessorNode {
public:
    AudioProcessorNode() = default;
    virtual ~AudioProcessorNode() noexcept = default;

    // Adds a node as a child of this node in the audio graph
    void AddChild(std::shared_ptr<AudioProcessorNode> child);
    
    // Clears all active modulations of the entire reachable subgraph starting at this node
    void ClearModulations() noexcept;

    // Clears the active modulations of this node
    virtual void ClearModulationsImpl() noexcept {};

    // Apply modulation to this node
    virtual void ApplyModulation(float amount, Modulation::Type modType) noexcept;

    // Process this node's audio (taking the childrens output as input to this node). Marks every visited node to avoid processing any node twice.
    AudioFrame GenerateFrame(const Audio::Preset::AudioPreset& preset);
    
    // Implementation specific node processing for the different effects/generators that derive from AudioProcessorNode
    virtual void ProcessFrame(AudioFrame& output) = 0;

    // Clears the markings on all nodes reachable from this node, to allow processing the next frame
    void ClearVisited() noexcept;

    Audio::Core::Gain gain;
    Audio::Core::Pan pan;
    bool isOn = true;
    Util::NormalizedFloat mix = 1.0f;

private:
    // Helper method that applies the current gain and pan on a frame and returns the result
    AudioFrame ApplyGainAndPan(const AudioFrame& frame) noexcept; 

    bool m_visited = false;
    AudioFrame m_cachedResult;

    std::unordered_set<std::shared_ptr<AudioProcessorNode>> m_children;
};

} // namespace Audio::Engine
