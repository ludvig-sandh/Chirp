#include "AudioProcessor.hpp"
#include "AudioEngine.hpp"
#include "AudioBackend.hpp"
#include "LFO.hpp"

void AudioProcessor::AddChild(std::shared_ptr<AudioProcessor> child) {
    m_children.insert(child);
}

void AudioProcessor::SetCallbackForReadingPreset(std::function<void(AudioProcessor *, const AudioPreset&)> callback) {
    m_presetCallback = callback;
}

void AudioProcessor::AddLFO(std::shared_ptr<LFO> lfo) {
    m_lfos.insert(lfo);
}

void AudioProcessor::Process(AudioBuffer& buffer, const AudioPreset& preset) {
    // Process all children first
    for (const std::shared_ptr<AudioProcessor>& child : m_children) {
        child->Process(buffer, preset);
    }

    // Process the output from the children via self, but first update any properties in response to GUI state
    if (m_presetCallback.has_value()) {
        m_presetCallback->operator()(this, preset);
    }

    const float *in = buffer.inputBuffer;
    float *out = buffer.outputBuffer;
    for (unsigned long i = 0; i < buffer.framesPerBuffer; i++) {
        // Update and apply LFOs, but first clear all modulations since they accumulate and reset within a single frame
        ClearModulations();
        for (std::shared_ptr<LFO> lfo : m_lfos) {
            if (lfo->callback.has_value()) {
                lfo->callback->operator()(lfo.get(), this);
            }
        }

        // Construct frame to process
        AudioBufferFrame frame;
        frame.inputFrame = in;
        frame.outputFrame = out;
        frame.framesPerBuffer = buffer.framesPerBuffer;
        frame.frameIdx = i;
        
        ProcessFrame(frame);
        ApplyGain(frame);

        // TODO: Set this to 'num channels' instead of magic numbers
        in += 2;
        out += 2;
    }
}

void AudioProcessor::ApplyGain(AudioBufferFrame& frame) {
    *frame.outputFrame *= gain.Get() * pan.GetLeftGain(); /* left */
    *(frame.outputFrame + 1) *= gain.Get() * pan.GetRightGain(); /* right */
}
