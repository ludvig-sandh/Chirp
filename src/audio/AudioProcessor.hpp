#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <functional>
#include <optional>

#include "Frequency.hpp"
#include "AudioBackend.hpp"

// Represents a node in a tree showing how audio is routed throughout the engine.
class AudioProcessor {
public:
    AudioProcessor() = default;
    virtual ~AudioProcessor() = default;

    void AddChild(std::shared_ptr<AudioProcessor> child);

    void SetCallbackForReadingPreset(std::function<void(AudioProcessor *, const AudioPreset&)> callback);

    void Process(const AudioBuffer& buffer, const AudioPreset& preset);

    virtual void ProcessImpl(const AudioBuffer &buffer) = 0;

private:
    std::unordered_set<std::shared_ptr<AudioProcessor>> m_children;
    std::optional<std::function<void(AudioProcessor *, const AudioPreset&)>> m_presetCallback;
};

/*
CLASS HIERARCHY:

AudioProcessor (base)

LFO (base)

Generator (base)
    is AudioProcessor

Oscillator
    is Generator
    is LFO
    has Waveform

Waveform (base)
    is Oscillator

Sine/Saw/Sq/...
    is Waveform

Envelope
    is LFO
*/