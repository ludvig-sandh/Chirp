#pragma once

#include <atomic>

struct AudioPreset {
    std::atomic<float> masterVolume { 0.05f };
    std::atomic<float> masterPan { 0.5f };

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
};