// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/modulation/LFO.hpp"
#include "audio/preset/AudioPreset.hpp"
#include <vector>
#include <memory>

// Fwd dec.
namespace Audio::Engine {
    class AudioProcessorNode;
}

namespace Audio::Modulation {

enum class Type {
    Pitch,
    Volume,
    Pan,
    Cutoff,
    Peaking
};

struct Route {
    std::shared_ptr<Audio::Modulation::LFO> source;
    std::shared_ptr<Audio::Engine::AudioProcessorNode> destination;
    Type modType;
    float amount;

    Route(std::shared_ptr<LFO> source, std::shared_ptr<Audio::Engine::AudioProcessorNode> destination, Type modType, float amount)
        : source(source)
        , destination(destination)
        , modType(modType)
        , amount(amount)
    {}
};

class Matrix {
public:
    Matrix() = default;

    void ClearRoutes();
    void AddRoute(Route route);
    void ApplyModulations();

private:
    std::vector<Route> m_routes;
};

} // namespace Audio::Modulation
