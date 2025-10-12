#pragma once

#include "AudioProcessor.hpp"

// Represents an AudioProcessor base class that specifically generates sound, in contrast to one that modifies sound (Effects)
class Generator : public AudioProcessor {
public:
    virtual ~Generator() = default;

    void ProcessImpl(const AudioBuffer &buffer) override;

    // Computes the next sample in the signal to generate
    virtual float GetNextSample() = 0;

    // TODO: Turn this pair into its own type
    float currentVolume = 0.0; // Rather smooth from 0 to selected volume than from full volume (click sound at startup)
    float targetVolume = 1.0;
};