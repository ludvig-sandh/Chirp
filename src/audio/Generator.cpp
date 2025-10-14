#include "Generator.hpp"

void Generator::ProcessFrame(AudioBufferFrame& frame) {
    // In order to avoid abrupt jumps in the signal due to volume changes, we must smooth changes out.
    float volumeStep = (targetVolume - currentVolume) / frame.framesPerBuffer;

    currentVolume += volumeStep;
    float value = GetNextSample() * currentVolume;
    *frame.outputFrame += value;     /* left */
    *(frame.outputFrame + 1) += value; /* right */
}