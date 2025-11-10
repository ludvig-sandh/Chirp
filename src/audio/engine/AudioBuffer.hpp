#pragma once

#include "AudioFrame.hpp"
#include <vector>

struct AudioBuffer {
    std::vector<AudioFrame> outputBuffer;
    size_t numFrames;

    AudioBuffer(size_t numFrames);

    AudioBuffer(float *initBuffer, size_t numFrames);
};
