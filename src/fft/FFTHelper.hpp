// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <vector>
#include <cstdint>

constexpr double MIN_DB = -100.0; // floor value for normalization
constexpr double MAX_DB = 0.0;    // top of range

namespace FFTHelper {
    // Computes the magnitude spectrum of the input signal and returns it in decibels.
    // The input vector is interpreted as a time-domain signal. An FFT is performed,
    // the magnitude of each frequency bin is taken, and then converted to dB.
    // Returns a shared_ptr to a vector of magnitudes in decibels.
    std::shared_ptr<std::vector<float>> ComputeFFTMagnitudeInDecibels(const std::vector<float>& window);
    
    // Helper: next power of 2
    size_t NextPow2(size_t v);
};
