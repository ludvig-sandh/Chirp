#include "Generator.hpp"

void Generator::ProcessImpl(const AudioBuffer &buffer) {
    // In order to avoid abrupt jumps in the signal due to volume changes, we must smooth changes out.
    float volumeStep = (targetVolume - currentVolume) / buffer.framesPerBuffer;

    float *out = buffer.outputBuffer;
    for (unsigned long i = 0; i < buffer.framesPerBuffer; i++) {
        currentVolume += volumeStep;
        float value = GetNextSample() * currentVolume;
        *out++ = value;  /* left */
        *out++ = value;  /* right */
    }
}