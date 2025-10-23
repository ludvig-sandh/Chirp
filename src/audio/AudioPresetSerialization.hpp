// SPDX-License-Identifier: MIT
// Copyright (c) 2025
// Author: Ludvig Sandh

#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include "AudioPreset.hpp"

using json = nlohmann::json;

namespace AudioPresetIO {

// Helper: converts preset to JSON
inline json ToJson(const AudioPreset& p) {
    json j;

    // Synth preset
    j["synthMasterVolume"] = p.synthMasterVolume.load();

    j["synthOscAWaveform"] = static_cast<int>(p.synthOscAWaveform.load());
    j["synthOscAOn"] = p.synthOscAOn.load();
    j["synthOscAVolume"] = p.synthOscAVolume.load();
    j["synthOscAPan"] = p.synthOscAPan.load();
    j["synthOscAOctave"] = p.synthOscAOctave.load();

    j["synthOscBWaveform"] = static_cast<int>(p.synthOscBWaveform.load());
    j["synthOscBOn"] = p.synthOscBOn.load();
    j["synthOscBVolume"] = p.synthOscBVolume.load();
    j["synthOscBPan"] = p.synthOscBPan.load();
    j["synthOscBOctave"] = p.synthOscBOctave.load();

    j["synthOscAttack"] = p.synthOscAttack.load();
    j["synthOscHold"] = p.synthOscHold.load();
    j["synthOscDec"] = p.synthOscDec.load();
    j["synthOscSus"] = p.synthOscSus.load();

    j["synthLpFilterOn"] = p.synthLpFilterOn.load();
    j["synthLpFilterMix"] = p.synthLpFilterMix.load();
    j["synthLpFilterCutoff"] = p.synthLpFilterCutoff.load();
    j["synthLpFilterQ"] = p.synthLpFilterQ.load();

    j["synthOscLpCutoffAttack"] = p.synthOscLpCutoffAttack.load();
    j["synthOscLpCutoffDec"] = p.synthOscLpCutoffDec.load();
    j["synthOscLpCutoffAmount"] = p.synthOscLpCutoffAmount.load();

    j["synthHpFilterOn"] = p.synthHpFilterOn.load();
    j["synthHpFilterMix"] = p.synthHpFilterMix.load();
    j["synthHpFilterCutoff"] = p.synthHpFilterCutoff.load();
    j["synthHpFilterQ"] = p.synthHpFilterQ.load();

    j["synthDelayOn"] = p.synthDelayOn.load();
    j["synthDelayType"] = static_cast<int>(p.synthDelayType.load());
    j["synthDelayMix"] = p.synthDelayMix.load();
    j["synthDelayTime"] = p.synthDelayTime.load();
    j["synthDelayFeedback"] = p.synthDelayFeedback.load();

    j["synthReverbOn"] = p.synthReverbOn.load();
    j["synthReverbFeedback"] = p.synthReverbFeedback.load();
    j["synthReverbDamp"] = p.synthReverbDamp.load();
    j["synthReverbWet"] = p.synthReverbWet.load();

    return j;
}

//--------------------------------
// Helper: JSON -> preset
//--------------------------------
inline void FromJson(const json& j, AudioPreset& p)
{
    auto get = [&](auto& field, const char* key, auto def) {
        field.store(j.value(key, def));
    };

    get(p.synthMasterVolume, "synthMasterVolume", 0.05f);

    p.synthOscAWaveform.store(static_cast<WaveformInfo::Type>(j.value("synthOscAWaveform", static_cast<int>(WaveformInfo::Type::Saw))));
    get(p.synthOscAOn, "synthOscAOn", true);
    get(p.synthOscAVolume, "synthOscAVolume", 0.7f);
    get(p.synthOscAPan, "synthOscAPan", 0.5f);
    get(p.synthOscAOctave, "synthOscAOctave", 5);

    p.synthOscBWaveform.store(static_cast<WaveformInfo::Type>(j.value("synthOscBWaveform", static_cast<int>(WaveformInfo::Type::Saw))));
    get(p.synthOscBOn, "synthOscBOn", false);
    get(p.synthOscBVolume, "synthOscBVolume", 0.7f);
    get(p.synthOscBPan, "synthOscBPan", 0.5f);
    get(p.synthOscBOctave, "synthOscBOctave", 5);

    get(p.synthOscAttack, "synthOscAttack", 0.0f);
    get(p.synthOscHold, "synthOscHold", 0.0f);
    get(p.synthOscDec, "synthOscDec", 0.0f);
    get(p.synthOscSus, "synthOscSus", 1.0f);

    get(p.synthLpFilterOn, "synthLpFilterOn", true);
    get(p.synthLpFilterMix, "synthLpFilterMix", 1.0f);
    get(p.synthLpFilterCutoff, "synthLpFilterCutoff", 5000.0f);
    get(p.synthLpFilterQ, "synthLpFilterQ", 0.707f);

    get(p.synthOscLpCutoffAttack, "synthOscLpCutoffAttack", 0.0f);
    get(p.synthOscLpCutoffDec, "synthOscLpCutoffDec", 0.0f);
    get(p.synthOscLpCutoffAmount, "synthOscLpCutoffAmount", 12.0f);

    get(p.synthHpFilterOn, "synthHpFilterOn", false);
    get(p.synthHpFilterMix, "synthHpFilterMix", 1.0f);
    get(p.synthHpFilterCutoff, "synthHpFilterCutoff", 1000.0f);
    get(p.synthHpFilterQ, "synthHpFilterQ", 0.707f);

    get(p.synthDelayOn, "synthDelayOn", false);
    p.synthDelayType.store(static_cast<FeedbackDelayInfo::Type>(j.value("synthDelayType", static_cast<int>(FeedbackDelayInfo::Type::Mono))));
    get(p.synthDelayMix, "synthDelayMix", 1.0f);
    get(p.synthDelayTime, "synthDelayTime", 0.2f);
    get(p.synthDelayFeedback, "synthDelayFeedback", 0.5f);

    get(p.synthReverbOn, "synthReverbOn", false);
    get(p.synthReverbFeedback, "synthReverbFeedback", 0.8f);
    get(p.synthReverbDamp, "synthReverbDamp", 0.2f);
    get(p.synthReverbWet, "synthReverbWet", 0.5f);
}

//--------------------------------
// File I/O helpers
//--------------------------------
inline bool SaveToFile(const AudioPreset& p, const std::string& filePath)
{
    std::ofstream file(filePath);
    if (!file.is_open())
        return false;

    json j = ToJson(p);
    file << j.dump(4); // Pretty print with 4 spaces
    return true;
}

inline bool LoadFromFile(AudioPreset& p, const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return false;

    json j;
    try {
        file >> j;
    } catch (...) {
        return false;
    }

    FromJson(j, p);
    return true;
}

} // namespace AudioPresetIO
