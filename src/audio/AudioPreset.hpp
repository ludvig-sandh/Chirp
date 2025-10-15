#pragma once

#include <atomic>

struct AudioPreset {
    std::atomic<float> masterVolume { 0.05 };
    std::atomic<float> masterPan { 0.5 };
};