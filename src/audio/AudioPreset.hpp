#pragma once

#include <atomic>

struct AudioPreset {
    std::atomic<float> volume { 0.05 };
};