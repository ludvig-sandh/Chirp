// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include "synchronization/ProducerConsumer.hpp"
#include "audio/engine/AudioBuffer.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <utility>

class FFTComputer {
public:
    std::shared_ptr<std::vector<float>> GetLastFFTResult() const;
    std::shared_ptr<Audio::Engine::AudioFrame> GetLastAudioLevels() const;

    // Called by audio thread to produce audio data
    void ProvideAudioBuffer(const Audio::Engine::AudioBuffer& buffer);

    // Called on a separate FFT thread to consume audio data
    void Start(std::atomic<bool>& running);

    // Forward to the producer consumer that the producer has stopped
    void FinishedProducing();

private:
    void StoreNewFFTResult(std::shared_ptr<std::vector<float>> result);
    void StoreNewAudioLevels(Audio::Engine::AudioFrame levels);

    Synchronization::ProducerConsumer<std::vector<float>> m_producerConsumer;

    std::vector<float> m_fftBuffer;

    std::shared_ptr<std::vector<float>> m_lastResult;
    std::shared_ptr<Audio::Engine::AudioFrame> m_lastAudioLevels;

    std::mutex m_resultMtx;
};
