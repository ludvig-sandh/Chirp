#pragma once

#include <atomic>
#include "Waveform.hpp"
#include "AppMode.hpp"

struct AudioPreset {
    std::atomic<AppMode> appMode { AppMode::Chirp };

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

    std::atomic<WaveformInfo::Type> oscAWaveform { WaveformInfo::Type::Sine };
    std::atomic<bool> oscAOn { true };
    std::atomic<float> oscAVolume { 0.7f };
    std::atomic<float> oscAPan { 0.5f };
};