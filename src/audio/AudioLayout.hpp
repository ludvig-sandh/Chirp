#pragma once

#include "AudioProcessor.hpp"
#include "AudioPreset.hpp"
#include <memory>

// Represents an audio processing graph along with a modulation matrix.
// The audio layout specifies the oscillators, effect chains, audio+modulation
// routing.
class AudioLayout {
public:
    virtual std::shared_ptr<AudioProcessor> GetRootNode() = 0;
    virtual void LoadPreset(AudioPreset& preset) = 0;
    void ClearAllModulations() {
        GetRootNode()->ClearModulations();
    }
    virtual void ApplyAllModulations() {}
};
