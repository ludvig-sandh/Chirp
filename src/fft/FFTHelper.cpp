// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "FFTHelper.hpp"
#include "audio/engine/AudioEngine.hpp"
#include "pocketfft_hdronly.h"

#include <cassert>
#include <complex>
#include <cmath>
#include <numbers>
#include <limits>

namespace FFTHelper {

size_t NextPow2(size_t v) {
    // Check for overflow
    if (v >= std::numeric_limits<std::size_t>::max()) {
        throw std::overflow_error("NextPow2 overflow for input");
    }

    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    if (std::numeric_limits<std::size_t>::max() >= std::numeric_limits<std::uint32_t>::max()) {
        v |= v >> 32;
    }

    v += 1;

    return v;
}

std::shared_ptr<std::vector<float>> ComputeFFTMagnitudeInDecibels(const std::vector<float>& window) {
    assert(!window.empty());

    const size_t Nwin = std::size(window);
    const size_t Nfft = NextPow2(Nwin);

    // Build Hann window and compute its coherent gain (sum of window)
    std::vector<double> hann(Nwin);
    for (int n = 0; n < Nwin; ++n) {
        hann[n] = 0.5 * (1.0 - std::cos(2.0 * std::numbers::pi * static_cast<double>(n) / (Nwin - 1)));
    }

    const double winSum = [&hann]() -> double {
        double sum = std::accumulate(hann.begin(), hann.end(), 0.0);

        // Avoid divide by zero
        if (sum == 0.0) {
            return 1.0;
        }
        return sum;
    }();

    // Prepare real input (double), apply window, zero-pad up to Nfft
    std::vector<double> input_double(Nfft, 0.0);
    for (int n = 0; n < Nwin; ++n) {
        input_double[n] = static_cast<double>(window[n]) * hann[n];
    }

    // r2c setup (pocketfft_hdronly)
    const pocketfft::shape_t shape_in{Nfft};
    const pocketfft::stride_t stride_in{sizeof(double)};
    const pocketfft::stride_t stride_out{sizeof(std::complex<double>)};
    const pocketfft::shape_t axes{0};
    const bool forward{true};
    const double fct{1.0};

    // Output buffer must be at least Nfft elements; we will then resize to Nfft/2+1
    std::vector<std::complex<double>> fftOut(Nfft);
    pocketfft::r2c(shape_in, stride_in, stride_out, axes, forward, input_double.data(), fftOut.data(), fct);

    // Keep only unique bins for real-input FFT
    const size_t Nout = Nfft / 2 + 1;
    fftOut.resize(Nout);

    // Compute magnitude (or power), normalize by window gain and by Nfft if desired.
    const double windowGainComp = 1.0 / winSum;
    const double eps = 1e-12;

    auto result = std::make_shared<std::vector<float>>(Nout);
    for (int k = 0; k < Nout; ++k) {
        const double re = fftOut[k].real();
        const double im = fftOut[k].imag();
        const double mag = std::sqrt(re * re + im * im) * windowGainComp;
        const double db = 20.0 * std::log10(mag + eps);

        // Normalize dB -> [0, 1]
        double norm = (db - MIN_DB) / (MAX_DB - MIN_DB);
        norm = std::clamp(norm, 0.0, 1.0);

        (*result)[k] = static_cast<float>(norm);
    }

    return result;
}

}