// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "audio/modulation/LFO.hpp"
#include "audio/preset/AudioPreset.hpp"
#include <vector>
#include <memory>

// Fwd dec.
class AudioProcessorNode;

enum class ModulationType {
    Pitch,
    Volume,
    Pan,
    Cutoff,
    Peaking
};

struct ModulationRoute {
    std::shared_ptr<LFO> source;
    std::shared_ptr<AudioProcessorNode> destination;
    ModulationType modType;
    float amount;

    ModulationRoute(std::shared_ptr<LFO> source,
                    std::shared_ptr<AudioProcessorNode> destination,
                    ModulationType modType,
                    float amount)
        : source(source)
        , destination(destination)
        , modType(modType)
        , amount(amount)
    {}
};

class ModulationMatrix {
public:
    ModulationMatrix() = default;

    void ClearRoutes();
    void AddRoute(ModulationRoute route);
    void ApplyModulations();

private:
    std::vector<ModulationRoute> m_routes;
};
