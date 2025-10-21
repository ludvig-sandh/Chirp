#pragma once

#include <atomic>
#include "Waveform.hpp"
#include "AppMode.hpp"

struct AudioPreset {
    std::atomic<AppMode> appMode { AppMode::Chirp };

    // Settings for Chirp mode
    std::atomic<float> masterVolume { 0.05f };

    std::atomic<bool> chirpOn { true };
    std::atomic<float> chirpVolume { 1.0f };
    std::atomic<float> chirpPan { 0.5f };

    std::atomic<bool> noiseOn { true };
    std::atomic<float> noiseVolume { 0.4f };

    std::atomic<bool> lpFilterOn { true };
    std::atomic<float> lpFilterMix { 1.0f };
    std::atomic<float> lpFilterCutoff { 10000.0f };
    std::atomic<float> lpFilterQ { 0.707f };
    
    std::atomic<bool> hpFilterOn { false };
    std::atomic<float> hpFilterMix { 1.0f };
    std::atomic<float> hpFilterCutoff { 10000.0f };
    std::atomic<float> hpFilterQ { 0.707f };

    std::atomic<float> reverbFeedback { 0.8f };
    std::atomic<float> reverbDamp { 0.2f };
    std::atomic<float> reverbWet { 0.5f };
    std::atomic<bool> reverbOn { true };

    // Settings for synth mode
    std::atomic<WaveformInfo::Type> oscAWaveform { WaveformInfo::Type::Saw };
    std::atomic<bool> oscAOn { true };
    std::atomic<float> oscAVolume { 0.7f };
    std::atomic<float> oscAPan { 0.5f };
    std::atomic<int> oscAOctave { 5 };

    std::atomic<float> oscAAttack { 0.0f };
    std::atomic<float> oscAHold { 0.0f };
    std::atomic<float> oscADec { 0.0f };
    std::atomic<float> oscASus { 1.0f };

    // Non-settings. Used to communicate key-presses to audio engine
    std::atomic<bool> noteA5 { false };
    std::atomic<bool> noteAs5 { false };
    std::atomic<bool> noteB5 { false };
    std::atomic<bool> noteC5 { false };
    std::atomic<bool> noteCs5 { false };
    std::atomic<bool> noteD5 { false };
    std::atomic<bool> noteDs5 { false };
    std::atomic<bool> noteE5 { false };
    std::atomic<bool> noteF5 { false };
    std::atomic<bool> noteFs5 { false };
    std::atomic<bool> noteG5 { false };
    std::atomic<bool> noteGs5 { false };
};