#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <functional>
#include <optional>

#include "Frequency.hpp"
#include "AudioBackend.hpp"

class Envelope {
public:
    Envelope() : attack(0.0), hold(0.0), dec(0.0), sus(1.0) {} // "invisible" envelope, just lets the signal pass through
    Envelope(float attack, float hold, float dec, float sus) : attack(attack), hold(hold), dec(dec), sus(sus) {
        assert(sus >= 0.0 && sus <= 1.0 && "");
    }

    float GetAmplitude(float timeSinceStart) {
        if (timeSinceStart < attack) {
            return timeSinceStart / attack;
        }
        if (timeSinceStart < attack + hold) {
            return 1.0;
        }
        if (timeSinceStart < attack + hold + dec) {
            float timeSinceDecStart = timeSinceStart - attack - hold;
            return 1.0 - (1.0 - sus) * timeSinceDecStart / dec;
        }
        return sus;
    }

private:
    float attack;
    float hold;
    float dec;
    float sus;
};

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

class Waveform : public AudioProcessor {
public:
    Waveform(Frequency freq) : frequency(freq), m_envelope() {}
    Waveform(Frequency freq, Envelope env) : frequency(freq), m_envelope(env) {}
    virtual ~Waveform() = default;

    float GetNextSample();

    void ProcessImpl(const AudioBuffer &buffer) override;

    virtual float GetNextSampleImpl(float currentOffset) = 0;

    Frequency frequency;
    float currentVolume = 0.0; // Rather smooth from 0 to selected volume than from full volume (click sound at startup)
    float targetVolume = 1.0;

private:
    float m_currentOffset = 0.0;
    float m_timeSinceStart = 0.0;
    Envelope m_envelope;
};

class Sine : public Waveform {
public:
    Sine(Frequency freq) : Waveform(freq) {}
    Sine(Frequency freq, Envelope env) : Waveform(freq, env) {}

    float GetNextSampleImpl(float currentOffset) override;
};

class Square : public Waveform {
public:
    Square(Frequency freq) : Waveform(freq) {}
    Square(Frequency freq, Envelope env) : Waveform(freq, env) {}
    
    float GetNextSampleImpl(float currentOffset) override;
};

class Saw : public Waveform {
public:
    Saw(Frequency freq) : Waveform(freq) {}
    Saw(Frequency freq, Envelope env) : Waveform(freq, env) {}
    
    float GetNextSampleImpl(float currentOffset) override;
};