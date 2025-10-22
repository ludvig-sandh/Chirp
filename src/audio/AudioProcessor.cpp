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

AudioBuffer AudioProcessor::Process(size_t numFrames, const AudioPreset& preset) {
    if (m_visited) {
        assert(m_currentResult != nullptr);
        // Don't compute the result of this node twice
        return *m_currentResult;
    }
    m_visited = true;

    if (m_currentResult == nullptr || m_currentResult->outputBuffer.size() != numFrames) {
        m_currentResult = std::make_unique<AudioBuffer>(numFrames);
    }else {
        // Zero the buffer from last time
        for (size_t i = 0; i < numFrames; i++) {
            m_currentResult->outputBuffer[i].left = 0.0f;
            m_currentResult->outputBuffer[i].right = 0.0f;
        }
    }

    // Process all children first
    for (const std::shared_ptr<AudioProcessor>& child : m_children) {
        AudioBuffer childBuf = child->Process(numFrames, preset);
        m_currentResult->Add(childBuf);
    }

    // Process the output from the children via self, but first update any properties in response to GUI state
    if (m_presetCallback.has_value()) {
        m_presetCallback->operator()(this, preset);
    }

    for (AudioFrame& frame : m_currentResult->outputBuffer) {
        // Update and apply LFOs, but first clear all modulations since they accumulate and reset within a single frame
        ClearModulations();
        for (std::shared_ptr<LFO> lfo : m_lfos) {
            if (lfo->callback.has_value()) {
                lfo->callback->operator()(lfo.get(), this);
            }
        }

        AudioFrame bypassedFrame(frame);
        AudioFrame processedFrame(frame);

        if (isOn) {
            ProcessFrame(processedFrame);
            ApplyGainAndPan(processedFrame);
            frame = AudioFrame::Blend(processedFrame, bypassedFrame, mix);
        }else {
            frame = bypassedFrame;
        }

        frame.ClipToValidRange();
    }

    return *m_currentResult;
}

void AudioProcessor::ClearVisited() {
    m_visited = false;
    for (const std::shared_ptr<AudioProcessor>& child : m_children) {
        child->ClearVisited();
    }
}

void AudioProcessor::ApplyGainAndPan(AudioFrame& output) {
    output.left = gain.Apply(output.left);
    output.left = pan.ApplyLeftGain(output.left);
    output.right = gain.Apply(output.right);
    output.right = pan.ApplyRightGain(output.right);
}
