// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/effects/Reverb.hpp"
#include <numbers>

namespace Audio::Effects {

Reverb::Reverb() {
    // Initialize buffers
    for (auto d : combDelays) combBuffers.emplace_back(d);
    for (auto d : allpassDelays) allpassBuffers.emplace_back(d);
}

void Reverb::SetParams(float feedback, float damping, float wet) noexcept {
    feedback = feedback;
    damp = damping;
    wetMix = wet;
}

void Reverb::ProcessFrame(Audio::Engine::AudioFrame& output) {
    using AudioFrame = Audio::Engine::AudioFrame;

    AudioFrame in = output;

    // Parallel comb filters
    AudioFrame combOut;
    for (int i = 0; i < std::ssize(combBuffers); ++i) {
        std::vector<AudioFrame>& buf = combBuffers[i];
        int delay = combDelays[i];
        AudioFrame& filterState = combFilterState[i];

        AudioFrame y = buf[posComb[i]]; // delayed sample
        filterState = y * (1.0f - damp) + filterState * damp; // damping
        buf[posComb[i]] = in + filterState * feedback; // feedback write

        combOut += y;
        posComb[i] = (posComb[i] + 1) % delay;
    }

    // Normalize wet level
    combOut /= static_cast<float>(std::ssize(combBuffers));
    combOut *= 1.5f; // Restore energy after averaging

    // Series allpass filters for diffusion
    AudioFrame apOut = combOut;
    for (int i = 0; i < std::ssize(allpassBuffers); ++i) {
        std::vector<AudioFrame>& buf = allpassBuffers[i];
        int delay = allpassDelays[i];
        AudioFrame bufOut = buf[posAllpass[i]];

        // Allpass filter structure
        AudioFrame x = apOut + (-0.5f) * bufOut;
        buf[posAllpass[i]] = x;
        apOut = bufOut + x * 0.5f;

        posAllpass[i] = (posAllpass[i] + 1) % delay;
    }

    // Equal-power dry/wet mixing
    wetMix = std::clamp(wetMix, 0.0f, 1.0f);
    float dryGain = std::cos(wetMix * static_cast<float>(std::numbers::pi / 2.0));
    float wetGain = std::sin(wetMix * static_cast<float>(std::numbers::pi / 2.0));
    AudioFrame out = in * dryGain + apOut * wetGain;

    // Soft clip for safety (prevents runaway feedback)
    out.left = std::tanh(out.left); // keeps output in [-1, 1] smoothly
    out.right = std::tanh(out.right);

    output = out;
}

} // namespace Audio::Effects