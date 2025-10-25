// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <string>
#include <functional>
#include <optional>
#include <cassert>

#include "Waveform.hpp"

// Fwd dec.
class AudioProcessor;

struct LFOConfig {
    enum class Mode {
        Envelope,
        Periodic,
        Random
    };

    static inline const char *ModeNames[] = { "Envelope (restart on every note)", "Periodic (repeat continuously)", "Random" };

    enum class Destination {
        OscAVolume,
        OscAPan,
        OscAPitch,
        OscBVolume,
        OscBPan,
        OscBPitch,
        OscABVolume,
        OscABPan,
        OscABPitch,
        LPCutoff,
        LPPeaking,
        HPCutoff,
        HPPeaking,
    };

    struct DestinationInfo {
        std::string name;
        std::optional<std::string> format;
        float minValue;
        float maxValue;
        bool isLogarithmic;
    };

    static inline const std::array<std::pair<Destination, DestinationInfo>, 13> DESTINATION_INFOS {{
        { Destination::OscAVolume, { "Osc A Volume", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscAPan,    { "Osc A Pan", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscAPitch,  { "Osc A Pitch", "%.1f semitones", -60.0f, 60.0f, false } },
        { Destination::OscBVolume, { "Osc B Volume", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscBPan,    { "Osc B Pan", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscBPitch,  { "Osc B Pitch", "%.1f semitones", -60.0f, 60.0f, false } },
        { Destination::OscABVolume,{ "Osc AB Volume", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscABPan,   { "Osc AB Pan", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscABPitch, { "Osc AB Pitch", "%.1f semitones", -60.0f, 60.0f, false } },
        { Destination::LPCutoff,   { "LP Cutoff", "%.1f Hz", -60.0f, 60.0f, true } },
        { Destination::LPPeaking,  { "LP Peaking", std::nullopt, -3.0f, 3.0f, false } },
        { Destination::HPCutoff,   { "HP Cutoff", "%.1f Hz", -60.0f, 60.0f, true } },
        { Destination::HPPeaking,  { "HP Peaking", std::nullopt, -3.0f, 3.0f, false } },
    }};

    static const DestinationInfo& GetDestinationInfo(Destination dest) {
        for (const auto& entry : DESTINATION_INFOS) {
            if (entry.first == dest) {
                return entry.second;
            }
        }

#if !defined(NDEBUG)
        assert(false && "Invalid LFOConfig::Destination passed to GetDestinationInfo()");
#endif

        std::terminate(); // never recover, guaranteed crash
    }

    static std::vector<std::string> GetDestinationNames() {
        std::vector<std::string> names;
        names.reserve(DESTINATION_INFOS.size());

        for (const auto& entry : DESTINATION_INFOS) {
            names.push_back(std::string(entry.second.name));
        }

        return names;
    }

    bool on { false };
    Mode mode { LFOConfig::Mode::Periodic };
    Destination destination { LFOConfig::Destination::OscAVolume };
    float amount { 0.0f };
    float envAttack { 0.0f };
    float envHold { 0.0f };
    float envDec { 0.0f };
    float envSus { 1.0f };
    WaveformInfo::Type waveform { WaveformInfo::Type::Saw };
    float frequency { 1.0f };
    int lfoNum = 0; // Identifies the LFO
};

class LFO {
public:
    virtual ~LFO() = default;
    virtual float GetNextSample() = 0;
};