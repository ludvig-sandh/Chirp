#include "AudioFrame.hpp"

#include <algorithm>
#include <cmath>

// Clamp the amplitude to avoid the possibility of going deaf
void AudioFrame::ClipToValidRange() {
    left = std::clamp(left, -1.0f, 1.0f);
    right = std::clamp(right, -1.0f, 1.0f);
}

AudioFrame AudioFrame::Blend(const AudioFrame& processed, const AudioFrame& unprocessed, float mix) noexcept {
    mix = std::clamp(mix, 0.0f, 1.0f);

    // Equal-power dry/wet mixing
    float dryGain = std::cos(mix * static_cast<float>(std::numbers::pi / 2.0));
    float wetGain = std::sin(mix * static_cast<float>(std::numbers::pi / 2.0));
    float leftBlended = processed.left * wetGain + unprocessed.left * dryGain;
    float rightBlended = processed.right * wetGain + unprocessed.right * dryGain;
    return AudioFrame{leftBlended, rightBlended};
}