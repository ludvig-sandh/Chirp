// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <atomic>
#include <array>
#include "audio/core/Waveform.hpp"
#include "audio/effects/util/FeedbackDelayInfo.hpp"
#include "audio/modulation/LFO.hpp"

struct AudioPreset {
    // Synth settings
    std::atomic<float> synthMasterVolume { 0.05f };

    std::atomic<WaveformInfo::Type> synthOscAWaveform { WaveformInfo::Type::Saw };
    std::atomic<bool> synthOscAOn { true };
    std::atomic<float> synthOscAVolume { 0.7f };
    std::atomic<float> synthOscAPan { 0.5f };
    std::atomic<int> synthOscAOctave { 5 };

    std::atomic<WaveformInfo::Type> synthOscBWaveform { WaveformInfo::Type::Saw };
    std::atomic<bool> synthOscBOn { false };
    std::atomic<float> synthOscBVolume { 0.7f };
    std::atomic<float> synthOscBPan { 0.5f };
    std::atomic<int> synthOscBOctave { 5 };

    std::atomic<float> synthOscAttack { 0.0f };
    std::atomic<float> synthOscHold { 0.0f };
    std::atomic<float> synthOscDec { 0.0f };
    std::atomic<float> synthOscSus { 1.0f };
    std::atomic<float> synthOscRel { 0.0f };

    std::atomic<bool> synthLpFilterOn { true };
    std::atomic<float> synthLpFilterMix { 1.0f };
    std::atomic<float> synthLpFilterCutoff { 5000.0f };
    std::atomic<float> synthLpFilterQ { 0.707f };

    std::atomic<float> synthOscLpCutoffAttack { 0.0f };
    std::atomic<float> synthOscLpCutoffDec { 0.0f };
    std::atomic<float> synthOscLpCutoffAmount { 12.0f };
    
    std::atomic<bool> synthHpFilterOn { false };
    std::atomic<float> synthHpFilterMix { 1.0f };
    std::atomic<float> synthHpFilterCutoff { 1000.0f };
    std::atomic<float> synthHpFilterQ { 0.707f };

    std::atomic<bool> synthDelayOn { false };
    std::atomic<FeedbackDelayInfo::Type> synthDelayType { FeedbackDelayInfo::Type::Mono };
    std::atomic<float> synthDelayMix { 1.0f };
    std::atomic<float> synthDelayTime { 0.2f };
    std::atomic<float> synthDelayFeedback { 0.5f };

    std::atomic<bool> synthReverbOn { false };
    std::atomic<float> synthReverbFeedback { 0.8f };
    std::atomic<float> synthReverbDamp { 0.2f };
    std::atomic<float> synthReverbWet { 0.5f };

    std::atomic<bool> synthLFO1On { false };
    std::atomic<LFOConfig::Mode> synthLFO1Mode { LFOConfig::Mode::Periodic };
    std::atomic<LFOConfig::Destination> synthLFO1Destination { LFOConfig::Destination::OscAVolume };
    std::atomic<float> synthLFO1Amount { 0.0f };
    std::atomic<float> synthLFO1EnvAttack { 0.0f };
    std::atomic<float> synthLFO1EnvHold { 0.0f };
    std::atomic<float> synthLFO1EnvDec { 0.0f };
    std::atomic<float> synthLFO1EnvSus { 1.0f };
    std::atomic<WaveformInfo::Type> synthLFO1Waveform { WaveformInfo::Type::Saw };
    std::atomic<float> synthLFO1Frequency { 1.0f };

    std::atomic<bool> synthLFO2On { false };
    std::atomic<LFOConfig::Mode> synthLFO2Mode { LFOConfig::Mode::Periodic };
    std::atomic<LFOConfig::Destination> synthLFO2Destination { LFOConfig::Destination::OscAVolume };
    std::atomic<float> synthLFO2Amount { 0.0f };
    std::atomic<float> synthLFO2EnvAttack { 0.0f };
    std::atomic<float> synthLFO2EnvHold { 0.0f };
    std::atomic<float> synthLFO2EnvDec { 0.0f };
    std::atomic<float> synthLFO2EnvSus { 1.0f };
    std::atomic<WaveformInfo::Type> synthLFO2Waveform { WaveformInfo::Type::Saw };
    std::atomic<float> synthLFO2Frequency { 1.0f };

    // Non-settings. Used to communicate key-presses to audio engine
    static constexpr int NUM_KEYS = 88;

    // Indexed by keyboard note index from left to right (0: leftmost key, etc.)
    std::array<std::atomic<bool>, NUM_KEYS> noteStates; // Default init. to false
};