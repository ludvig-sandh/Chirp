// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "FFTHelper.hpp"
#include "AudioEngine.hpp"

#include "pocketfft_hdronly.h"

#include <cassert>
#include <complex>
#include <cmath>
#include <numbers>
#include <iostream>

// Helper: next power of two
size_t FFTHelper::next_pow2(size_t v) {
    if (v == 0) return 1;
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
#if SIZE_MAX > UINT32_MAX
    v |= v >> 32;
#endif
    return ++v;
}

std::shared_ptr<std::vector<float>> FFTHelper::ComputeFFTMagnitudeDB(const std::vector<float>& window) {
    assert(!window.empty());

    const size_t Nwin = window.size();
    size_t Nfft = next_pow2(Nwin);

    // Build Hann window and compute its coherent gain (sum of window)
    std::vector<double> hann(Nwin);
    double winSum = 0.0;
    for (size_t n = 0; n < Nwin; ++n) {
        hann[n] = 0.5 * (1.0 - std::cos(2.0 * std::numbers::pi * double(n) / double(Nwin - 1)));
        winSum += hann[n];
    }

    // Avoid divide by zero
    if (winSum == 0.0) winSum = 1.0;

    // Prepare real input (double), apply window, zero-pad up to Nfft
    std::vector<double> input_double(Nfft, 0.0);
    for (size_t n = 0; n < Nwin; ++n) {
        input_double[n] = static_cast<double>(window[n]) * hann[n];
    }

    // r2c setup (pocketfft_hdronly)
    pocketfft::shape_t shape_in{Nfft};
    pocketfft::stride_t stride_in{sizeof(double)};
    pocketfft::stride_t stride_out{sizeof(std::complex<double>)};
    pocketfft::shape_t axes{0};
    bool forward{true};
    double fct{1.0};

    // Output buffer must be at least Nfft elements; we will then resize to Nfft/2+1
    std::vector<std::complex<double>> fftOut(Nfft);
    pocketfft::r2c(shape_in, stride_in, stride_out, axes, forward,
        input_double.data(), fftOut.data(), fct);

    // Keep only unique bins for real-input FFT
    size_t Nout = Nfft / 2 + 1;
    fftOut.resize(Nout);

    // Compute magnitude (or power), normalize by window gain and by Nfft if desired.
    const double windowGainComp = 1.0 / winSum;
    const double eps = 1e-12;

    auto result = std::make_shared<std::vector<float>>(Nout);
    for (size_t k = 0; k < Nout; ++k) {
        double re = fftOut[k].real();
        double im = fftOut[k].imag();
        double mag = std::sqrt(re * re + im * im);
        mag *= windowGainComp;

        double db = 20.0 * std::log10(mag + eps);

        // Normalize dB -> [0, 1]
        double norm = (db - MIN_DB) / (MAX_DB - MIN_DB);
        norm = std::clamp(norm, 0.0, 1.0);

        (*result)[k] = static_cast<float>(norm);
    }

    return result;
}