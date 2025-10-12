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

float Waveform::GetNextSample() {
    float dt = 1.0 / SAMPLE_RATE;
    m_timeSinceStart += dt;
    float dOffset = dt * frequency.GetAbsolute();
    m_currentOffset += dOffset;

    // Loop back to always be in range [0, 1]
    m_currentOffset -= static_cast<int>(m_currentOffset);
    
    return GetNextSampleImpl(m_currentOffset) * m_envelope.GetAmplitude(m_timeSinceStart);
}

void Waveform::ProcessImpl(const AudioBuffer &buffer) {
    float *out = buffer.outputBuffer;
    for (unsigned long i = 0; i < buffer.framesPerBuffer; i++) {
        float value = GetNextSample() * outputVolume;
        *out++ = value;  /* left */
        *out++ = value;  /* right */
    }
}

float Sine::GetNextSampleImpl(float currentOffset) {
    return std::sin(currentOffset * 2.0 * M_PI);
}

float Square::GetNextSampleImpl(float currentOffset) {
    return currentOffset >= 0.5 ? 1.0 : 0.0;
}

float Saw::GetNextSampleImpl(float currentOffset) {
    return currentOffset;
}