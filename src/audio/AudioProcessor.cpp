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

    float *out = buffer.outputBuffer;
    for (unsigned long i = 0; i < buffer.framesPerBuffer; i++) {
        // Update and apply LFOs, but first clear all modulations since they accumulate and reset within a single frame
        ClearModulations();
        for (std::shared_ptr<LFO> lfo : m_lfos) {
            if (lfo->callback.has_value()) {
                lfo->callback->operator()(lfo.get(), this);
            }
        }

        float leftOutput = *out;
        float rightOutput = *(out + 1);
        AudioBufferFrame bypassedFrame{leftOutput, rightOutput};
        AudioBufferFrame processedFrame(bypassedFrame);

        AudioBufferFrame result;
        if (isOn) {
            ProcessFrame(processedFrame);
            ApplyGainAndPan(processedFrame);
            result = AudioBufferFrame::Blend(processedFrame, bypassedFrame, mix);
        }else {
            result = bypassedFrame;
        }
        
        // Write back output to buffer
        *out++ = result.left;
        *out++ = result.right;
    }
}

void AudioProcessor::ApplyGainAndPan(AudioBufferFrame& output) {
    output.left *= gain.Get() * pan.GetLeftGain();
    output.right *= gain.Get() * pan.GetRightGain();
}
