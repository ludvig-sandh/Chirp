#include "audio/engine/AudioBuffer.hpp"

AudioBuffer::AudioBuffer(size_t numFrames) : outputBuffer(numFrames), numFrames(numFrames) {}

AudioBuffer::AudioBuffer(float *initBuffer, size_t numFrames)
    : outputBuffer(numFrames)
    , numFrames(numFrames)
{
    float *out = initBuffer;
    for (size_t i = 0; i < numFrames; i++) {
        outputBuffer[i].left = *out++;
        outputBuffer[i].right = *out++;
    }
}
