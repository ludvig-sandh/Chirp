#pragma once

#include "LFO.hpp"
#include "AudioPreset.hpp"
#include <vector>
#include <memory>

// Fwd dec.
class AudioProcessor;

enum class ModulationType {
    Pitch,
    Volume,
    Pan,
    Cutoff,
    Peaking
};

struct ModulationRoute {
    std::shared_ptr<LFO> source;
    std::shared_ptr<AudioProcessor> destination;
    ModulationType modType;
    float amount;

    ModulationRoute(std::shared_ptr<LFO> source,
                    std::shared_ptr<AudioProcessor> destination,
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
