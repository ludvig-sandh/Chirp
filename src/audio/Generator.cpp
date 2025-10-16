#include "Generator.hpp"

void Generator::ProcessFrame(AudioBufferFrame& output) {
    float value = GetNextSample();
    output.left += value;
    output.right += value;
}