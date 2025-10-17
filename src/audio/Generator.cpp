#include "Generator.hpp"

Generator::Generator() {
    m_headroom.SetDecibels(s_headroomLeveldB);
}

void Generator::ProcessFrame(AudioBufferFrame& output) {
    float value = m_headroom.Apply(GetNextSample());
    output.left += value;
    output.right += value;
}