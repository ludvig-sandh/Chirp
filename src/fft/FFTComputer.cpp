// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "FFTComputer.hpp"
#include "FFTHelper.hpp"

#include <cassert>
#include <iostream>
#include <cmath>

std::shared_ptr<std::vector<float>> FFTComputer::GetLastFFTResult() const {
    return m_lastResult.load(std::memory_order_acquire);
}

std::shared_ptr<AudioFrame> FFTComputer::GetLastAudioLevels() const {
    return m_lastAudioLevels.load(std::memory_order_acquire);
}

void FFTComputer::StoreNewFFTResult(std::shared_ptr<std::vector<float>> result) {
    m_lastResult.store(result, std::memory_order_release);
}

void FFTComputer::StoreNewAudioLevels(AudioFrame result) {
    m_lastAudioLevels.store(std::make_shared<AudioFrame>(result), std::memory_order_release);
}

void FFTComputer::ProvideAudioBuffer(const AudioBuffer& buffer) {
    // Copy over the output buffer into a vector we can "produce"
    std::unique_ptr<std::vector<float>> output = std::make_unique<std::vector<float>>();
    output->reserve(buffer.numFrames);

    AudioFrame rms{0.0f, 0.0f};
    for (const AudioFrame& frame : buffer.outputBuffer) {
        // Sum of squares (RMS)
        rms.left += frame.left * frame.left;
        rms.right += frame.right * frame.right;
        
        output->push_back((frame.left + frame.right) / 2.0f); // Average across channels
    }

    // Mean and root (RMS)
    rms.left /= static_cast<float>(buffer.numFrames);
    rms.right /= static_cast<float>(buffer.numFrames);
    rms.left = std::sqrtf(rms.left);
    rms.right = std::sqrtf(rms.right);

    StoreNewAudioLevels(rms);
    m_producerConsumer.Produce(std::move(output));
}

void FFTComputer::Start(std::atomic<bool>& running) {
    while (running.load()) {
        // Continuously collect audio data from the audio engine.
        // When a large enough window has been collected, compute fft and store result.
        std::unique_ptr<std::vector<float>> audioToExtend = m_producerConsumer.Consume();
        if (!audioToExtend) {
            // The producer has stopped, which means the application has probably closed (running = false)
            continue;
        }

        // Extend fft buffer with the new audio chunk
        m_fftBuffer.reserve(m_fftBuffer.size() + distance(audioToExtend->begin(), audioToExtend->end()));
        m_fftBuffer.insert(m_fftBuffer.end(), audioToExtend->begin(), audioToExtend->end());

        if (m_fftBuffer.size() > 1024) {
            std::shared_ptr<std::vector<float>> fft_magnitude = FFTHelper::ComputeFFTMagnitudeDB(m_fftBuffer);

            // Pop oldest audio chunk from the fft buffer
            m_fftBuffer.erase(m_fftBuffer.begin(), m_fftBuffer.begin() + audioToExtend->size());
            StoreNewFFTResult(fft_magnitude);
        }
    }

    m_producerConsumer.Close();
}

void FFTComputer::FinishedProducing() {
    m_producerConsumer.Close();
}