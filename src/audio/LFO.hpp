// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <string>
#include <functional>
#include <optional>
#include <cassert>

// Fwd dec.
class AudioProcessor;

namespace LFOConfig {
    enum class Mode {
        PlayOnNote,
        Repeat
    };

    inline constexpr const char *ModeNames[] = { "Restart on every note", "Repeat continuously" };

    
    enum class Shape {
        Envelope,
        Waveform
    };

    inline constexpr const char *ShapeNames[] = { "Envelope", "Waveform" };


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

    inline const std::array<std::pair<Destination, DestinationInfo>, 13> DESTINATION_INFOS {{
        { Destination::OscAVolume, { "Osc A Volume", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscAPan,    { "Osc A Pan", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscAPitch,  { "Osc A Pitch", "%.1f semitones", -60.0f, 60.0f, false } },
        { Destination::OscBVolume, { "Osc B Volume", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscBPan,    { "Osc B Pan", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscBPitch,  { "Osc B Pitch", "%.1f semitones", -60.0f, 60.0f, false } },
        { Destination::OscABVolume,{ "Osc AB Volume", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscABPan,   { "Osc AB Pan", std::nullopt, -1.0f, 1.0f, false } },
        { Destination::OscABPitch, { "Osc AB Pitch", "%.1f semitones", -60.0f, 60.0f, false } },
        { Destination::LPCutoff,   { "LP Cutoff", "%.1f Hz", 20.0f, 20000.0f, true } },
        { Destination::LPPeaking,  { "LP Peaking", std::nullopt, 0.1f, 3.0f, false } },
        { Destination::HPCutoff,   { "HP Cutoff", "%.1f Hz", 20.0f, 20000.0f, true } },
        { Destination::HPPeaking,  { "HP Peaking", std::nullopt, 0.1f, 3.0f, false } },
    }};

    inline const DestinationInfo& GetDestinationInfo(Destination dest) {
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

    inline std::vector<std::string> GetDestinationNames() {
        std::vector<std::string> names;
        names.reserve(DESTINATION_INFOS.size());

        for (const auto& entry : DESTINATION_INFOS) {
            names.push_back(std::string(entry.second.name));
        }

        return names;
    }
}

class LFO {
public:
    virtual ~LFO() = default;
    virtual float GetNextSample() = 0;

    // Callback for applying the lfo
    std::optional<std::function<void(LFO *, AudioProcessor *)>> callback;
};