#include "Reverb.hpp"

#include <numbers>

Reverb::Reverb() {
    // Choose reasonable delay lengths (prime numbers help)
    combDelays = {1116, 1188, 1277, 1356};
    allpassDelays = {225, 556};

    // Initialize buffers
    for (auto d : combDelays) combBuffers.emplace_back(d, 0.0f);
    for (auto d : allpassDelays) allpassBuffers.emplace_back(d, 0.0f);
}

void Reverb::SetParams(float roomSize, float damping, float wet) {
    feedback = roomSize; // 0.5–0.9 typical
    damp = damping;      // 0–1 lowpass factor inside combs
    wetMix = wet;        // 0–1 dry/wet
}

void Reverb::ProcessFrame(AudioBufferFrame& output) {
    float in = output.left;

    // --- Parallel comb filters ---
    float combOut = 0.0f;
    for (size_t i = 0; i < combBuffers.size(); ++i)
    {
        auto& buf = combBuffers[i];
        int delay = combDelays[i];
        float& filterState = combFilterState[i];

        float y = buf[posComb[i]];                           // delayed sample
        filterState = y * (1.0f - damp) + filterState * damp; // damping
        buf[posComb[i]] = in + filterState * feedback;        // feedback write

        combOut += y;
        posComb[i] = (posComb[i] + 1) % delay;
    }

    // --- Normalize wet level ---
    combOut /= static_cast<float>(combBuffers.size());
    combOut *= 1.5f;  // restore energy after averaging

    // --- Series allpass filters for diffusion ---
    float apOut = combOut;
    for (size_t i = 0; i < allpassBuffers.size(); ++i)
    {
        auto& buf = allpassBuffers[i];
        int delay = allpassDelays[i];
        float bufOut = buf[posAllpass[i]];

        // allpass filter structure
        float x = apOut + (-0.5f) * bufOut;
        buf[posAllpass[i]] = x;
        apOut = bufOut + x * 0.5f;

        posAllpass[i] = (posAllpass[i] + 1) % delay;
    }

    // --- Equal-power dry/wet mixing ---
    wetMix = std::clamp(wetMix, 0.0f, 1.0f);
    float dryGain = std::cos(wetMix * static_cast<float>(std::numbers::pi / 2.0));
    float wetGain = std::sin(wetMix * static_cast<float>(std::numbers::pi / 2.0));
    float out = in * dryGain + apOut * wetGain;

    // --- Soft clip for safety (prevents runaway feedback) ---
    out = std::tanh(out); // keeps output in [-1, 1] smoothly


    output.left  = out;
    output.right = out;
}