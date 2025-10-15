#include "Generator.hpp"

void Generator::ProcessFrame(AudioBufferFrame& frame) {
    float value = GetNextSample();
    *frame.outputFrame += value;     /* left */
    *(frame.outputFrame + 1) += value; /* right */
}