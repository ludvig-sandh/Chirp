// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "modulation/ModulationMatrix.hpp"
#include "engine/AudioProcessor.hpp"

void ModulationMatrix::ClearRoutes() {
    m_routes.clear();
}

void ModulationMatrix::AddRoute(ModulationRoute route) {
    m_routes.push_back(route);
}

void ModulationMatrix::ApplyModulations() {
    // Cache for one audio sample frame
    std::unordered_map<LFO*, float> cachedValues;
    cachedValues.reserve(m_routes.size());

    for (auto& route : m_routes) {
        LFO* src = route.source.get();

        // Look up or compute
        auto it = cachedValues.find(src);
        if (it == cachedValues.end()) {
            // First time this LFO is used this sample -> evaluate it
            float next = src->GetNextSample();
            it = cachedValues.emplace(src, next).first;
        }

        float amount = route.amount * it->second;
        route.destination->ApplyModulation(amount, route.modType);
    }
}