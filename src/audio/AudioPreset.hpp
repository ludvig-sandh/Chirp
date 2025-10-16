#pragma once

#include <atomic>

struct AudioPreset {
    std::atomic<float> masterVolume { 0.05f };
    std::atomic<float> masterPan { 0.5f };
    std::atomic<float> lpFilterCutoff { 10000.0f };
    std::atomic<int> lpFilterSteepness { 1 };
    std::atomic<bool> lpFilterOn { true };
    std::atomic<float> lpFilterMix { 1.0f };
};