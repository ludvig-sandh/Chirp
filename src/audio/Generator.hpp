#pragma once

#include "AudioProcessor.hpp"

// Represents an AudioProcessor base class that specifically generates sound, in contrast to one that modifies sound (Effects)
class Generator : public AudioProcessor {
public:
    virtual ~Generator() = default;

    void ProcessFrame(AudioBufferFrame& output) override;

    // Computes the next sample in the signal to generate
    virtual float GetNextSample() = 0;
};