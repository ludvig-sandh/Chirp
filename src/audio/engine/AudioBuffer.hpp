#pragma once

#include "AudioFrame.hpp"
#include <vector>

struct AudioBuffer {
    std::vector<AudioFrame> data;

    AudioBuffer(int numFrames) : data(numFrames) {}

    AudioBuffer(float *initBuffer, int numFrames) : data(numFrames) {
        for (auto &frame : data) {
            frame.left = *initBuffer++;
            frame.right = *initBuffer++;
        }
    }

    // Let it behave like a container
    auto begin() noexcept { return data.begin(); }
    auto end() noexcept { return data.end(); }
    auto begin() const noexcept { return data.begin(); }
    auto end() const noexcept { return data.end(); }

    auto size() const noexcept { return std::ssize(data); }

    AudioFrame& operator[](int i) { return data[i]; }
    const AudioFrame& operator[](int i) const { return data[i]; }
};
