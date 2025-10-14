#include "Mixer.hpp"

void Mixer::ClearModulations() {

}

void Mixer::ProcessFrame(AudioBufferFrame& frame) {
    float volumeStep = (targetGain - m_currentGain) / frame.framesPerBuffer;
    m_currentGain += volumeStep;

    *frame.outputFrame *= m_currentGain; /* left */
    *(frame.outputFrame + 1) *= m_currentGain; /* right */
}
