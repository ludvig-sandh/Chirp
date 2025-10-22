#include "Generator.hpp"

Generator::Generator() {
    m_headroom.SetDecibels(s_headroomLeveldB);
}

void Generator::ProcessFrame(AudioFrame& output) {
    float value = m_headroom.Apply(GetNextSample());
    output += value;
}