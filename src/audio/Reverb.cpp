// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "Reverb.hpp"

#include <numbers>

Reverb::Reverb()
    : combDelays{1116, 1188, 1277, 1356} // Choose reasonable delay lengths (prime numbers help)
    , allpassDelays{225, 556}
    , combFilterState(4) {

    // Initialize buffers
    for (auto d : combDelays) combBuffers.emplace_back(d, AudioFrame());
    for (auto d : allpassDelays) allpassBuffers.emplace_back(d, AudioFrame());
}

void Reverb::SetParams(float roomSize, float damping, float wet) {
    feedback = roomSize; // 0.5–0.9 typical
    damp = damping;      // 0–1 lowpass factor inside combs
    wetMix = wet;        // 0–1 dry/wet
}

void Reverb::ProcessFrame(AudioFrame& output) {
    AudioFrame in = output;

    // --- Parallel comb filters ---
    AudioFrame combOut;
    for (size_t i = 0; i < combBuffers.size(); ++i)
    {
        std::vector<AudioFrame>& buf = combBuffers[i];
        int delay = combDelays[i];
        AudioFrame& filterState = combFilterState[i];

        AudioFrame y = buf[posComb[i]];                           // delayed sample
        filterState = y * (1.0f - damp) + filterState * damp; // damping
        buf[posComb[i]] = in + filterState * feedback;        // feedback write

        combOut += y;
        posComb[i] = (posComb[i] + 1) % delay;
    }

    // --- Normalize wet level ---
    combOut /= static_cast<float>(combBuffers.size());
    combOut *= 1.5f;  // restore energy after averaging

    // --- Series allpass filters for diffusion ---
    AudioFrame apOut = combOut;
    for (size_t i = 0; i < allpassBuffers.size(); ++i)
    {
        std::vector<AudioFrame>& buf = allpassBuffers[i];
        int delay = allpassDelays[i];
        AudioFrame bufOut = buf[posAllpass[i]];

        // allpass filter structure
        AudioFrame x = apOut + (-0.5f) * bufOut;
        buf[posAllpass[i]] = x;
        apOut = bufOut + x * 0.5f;

        posAllpass[i] = (posAllpass[i] + 1) % delay;
    }

    // --- Equal-power dry/wet mixing ---
    wetMix = std::clamp(wetMix, 0.0f, 1.0f);
    float dryGain = std::cos(wetMix * static_cast<float>(std::numbers::pi / 2.0));
    float wetGain = std::sin(wetMix * static_cast<float>(std::numbers::pi / 2.0));
    AudioFrame out = in * dryGain + apOut * wetGain;

    // --- Soft clip for safety (prevents runaway feedback) ---
    out.left = std::tanh(out.left); // keeps output in [-1, 1] smoothly
    out.right = std::tanh(out.right);

    output = out;
}