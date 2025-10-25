// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include "preset/AudioPreset.hpp"

using json = nlohmann::json;

namespace AudioPresetIO {

// Helper: converts preset to JSON
inline json ToJson(const AudioPreset& p) {
    json j;

    // Synth preset
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

    j["synthLFO1On"] = p.synthLFO1On.load();
    j["synthLFO1Mode"] = static_cast<int>(p.synthLFO1Mode.load());
    j["synthLFO1Destination"] = static_cast<int>(p.synthLFO1Destination.load());
    j["synthLFO1Amount"] = p.synthLFO1Amount.load();
    j["synthLFO1EnvAttack"] = p.synthLFO1EnvAttack.load();
    j["synthLFO1EnvHold"] = p.synthLFO1EnvHold.load();
    j["synthLFO1EnvDec"] = p.synthLFO1EnvDec.load();
    j["synthLFO1EnvSus"] = p.synthLFO1EnvSus.load();
    j["synthLFO1Waveform"] = static_cast<int>(p.synthLFO1Waveform.load());
    j["synthLFO1Frequency"] = p.synthLFO1Frequency.load();

    j["synthLFO2On"] = p.synthLFO2On.load();
    j["synthLFO2Mode"] = static_cast<int>(p.synthLFO2Mode.load());
    j["synthLFO2Destination"] = static_cast<int>(p.synthLFO2Destination.load());
    j["synthLFO2Amount"] = p.synthLFO2Amount.load();
    j["synthLFO2EnvAttack"] = p.synthLFO2EnvAttack.load();
    j["synthLFO2EnvHold"] = p.synthLFO2EnvHold.load();
    j["synthLFO2EnvDec"] = p.synthLFO2EnvDec.load();
    j["synthLFO2EnvSus"] = p.synthLFO2EnvSus.load();
    j["synthLFO2Waveform"] = static_cast<int>(p.synthLFO2Waveform.load());
    j["synthLFO2Frequency"] = p.synthLFO2Frequency.load();

    return j;
}

// Helper: converts JSON to preset
inline void FromJson(const json& j, AudioPreset& p)
{
    auto get = [&](auto& field, const char* key, auto def) {
        field.store(j.value(key, def));
    };

    p.synthOscAWaveform.store(static_cast<WaveformInfo::Type>(
        j.value("synthOscAWaveform", static_cast<int>(WaveformInfo::Type::Saw))
    ));
    get(p.synthOscAOn, "synthOscAOn", true);
    get(p.synthOscAVolume, "synthOscAVolume", 0.7f);
    get(p.synthOscAPan, "synthOscAPan", 0.5f);
    get(p.synthOscAOctave, "synthOscAOctave", 5);

    p.synthOscBWaveform.store(static_cast<WaveformInfo::Type>(
        j.value("synthOscBWaveform", static_cast<int>(WaveformInfo::Type::Saw))
    ));
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
    p.synthDelayType.store(static_cast<FeedbackDelayInfo::Type>(
        j.value("synthDelayType", static_cast<int>(FeedbackDelayInfo::Type::Mono))
    ));
    get(p.synthDelayMix, "synthDelayMix", 1.0f);
    get(p.synthDelayTime, "synthDelayTime", 0.2f);
    get(p.synthDelayFeedback, "synthDelayFeedback", 0.5f);

    get(p.synthReverbOn, "synthReverbOn", false);
    get(p.synthReverbFeedback, "synthReverbFeedback", 0.8f);
    get(p.synthReverbDamp, "synthReverbDamp", 0.2f);
    get(p.synthReverbWet, "synthReverbWet", 0.5f);

    
    get(p.synthLFO1On, "synthLFO1On", false);
    p.synthLFO1Mode.store(static_cast<LFOConfig::Mode>(
        j.value("synthLFO1Mode", static_cast<int>(LFOConfig::Mode::Periodic))
    ));
    p.synthLFO1Destination.store(static_cast<LFOConfig::Destination>(
        j.value("synthLFO1Destination", static_cast<int>(LFOConfig::Destination::OscAVolume))
    ));
    get(p.synthLFO1Amount, "synthLFO1Amount", 0.0f);
    get(p.synthLFO1EnvAttack, "synthLFO1EnvAttack", 0.0f);
    get(p.synthLFO1EnvHold, "synthLFO1EnvHold", 0.0f);
    get(p.synthLFO1EnvDec, "synthLFO1EnvDec", 0.0f);
    get(p.synthLFO1EnvSus, "synthLFO1EnvSus", 1.0f);
    p.synthLFO1Waveform.store(static_cast<WaveformInfo::Type>(
        j.value("synthLFO1Waveform", static_cast<int>(WaveformInfo::Type::Saw))
    ));
    get(p.synthLFO1Frequency, "synthLFO1Frequency", 1.0f);
    
    
    get(p.synthLFO2On, "synthLFO2On", false);
    p.synthLFO2Mode.store(static_cast<LFOConfig::Mode>(
        j.value("synthLFO2Mode", static_cast<int>(LFOConfig::Mode::Periodic))
    ));
    p.synthLFO2Destination.store(static_cast<LFOConfig::Destination>(
        j.value("synthLFO2Destination", static_cast<int>(LFOConfig::Destination::OscAVolume))
    ));
    get(p.synthLFO2Amount, "synthLFO2Amount", 0.0f);
    get(p.synthLFO2EnvAttack, "synthLFO2EnvAttack", 0.0f);
    get(p.synthLFO2EnvHold, "synthLFO2EnvHold", 0.0f);
    get(p.synthLFO2EnvDec, "synthLFO2EnvDec", 0.0f);
    get(p.synthLFO2EnvSus, "synthLFO2EnvSus", 1.0f);
    p.synthLFO2Waveform.store(static_cast<WaveformInfo::Type>(
        j.value("synthLFO2Waveform", static_cast<int>(WaveformInfo::Type::Saw))
    ));
    get(p.synthLFO2Frequency, "synthLFO2Frequency", 1.0f);
}

// File I/O helpers
inline bool SaveToFile(const AudioPreset& p, const std::string& filePath)
{
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    json j = ToJson(p);
    file << j.dump(4); // Pretty print with 4 spaces
    return true;
}

inline bool LoadFromFile(AudioPreset& p, const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

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
