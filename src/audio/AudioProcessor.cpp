#include "AudioProcessor.hpp"
#include "AudioEngine.hpp"

void AudioProcessor::AddChild(std::shared_ptr<AudioProcessor> child) {
    m_children.insert(child);
}

void AudioProcessor::SetCallbackForReadingPreset(std::function<void(AudioProcessor *, const AudioPreset&)> callback) {
    m_presetCallback = callback;
}

void AudioProcessor::Process(const AudioBuffer& buffer, const AudioPreset& preset) {
    // Process all children first
    for (const std::shared_ptr<AudioProcessor>& child : m_children) {
        child->Process(buffer, preset);
    }

    // Process the output from the children via self, but first update any properties in response to GUI state
    if (m_presetCallback.has_value()) {
        m_presetCallback->operator()(this, preset);
    }

    ProcessImpl(buffer);
}
