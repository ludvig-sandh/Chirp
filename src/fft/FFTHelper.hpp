// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <vector>
#include <cstdint>

namespace FFT::Helper {

// Computes the magnitude spectrum of the input signal and returns it in decibels.
// The input vector is interpreted as a time-domain signal. An FFT is performed,
// the magnitude of each frequency bin is taken, and then converted to dB.
// Returns a shared_ptr to a vector of magnitudes in decibels.
std::shared_ptr<std::vector<float>> ComputeFFTMagnitudeInDecibels(const std::vector<float>& window);

// Helper: next power of 2
size_t NextPow2(size_t v);

} // namespace FFT:Helper
