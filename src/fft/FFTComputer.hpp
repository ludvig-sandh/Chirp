#pragma once

#include "ProducerConsumer.hpp"
#include "AudioBackend.hpp"
#include <vector>
#include <memory>
#include <mutex>

class FFTComputer {
public:
    FFTComputer() : m_lastResult(std::make_shared<std::vector<float>>(256, 0.0)) {}

    std::shared_ptr<std::vector<float>> GetLastResult() const;

    // Called by audio thread to produce audio data
    void ProvideAudioBuffer(const AudioBuffer& buffer);

    // Called on a separate FFT thread to consume audio data
    void Start(std::atomic<bool>& running);

    // Forward to the producer consumer that the producer has stopped
    void FinishedProducing();

private:
    void StoreNewResult(std::shared_ptr<std::vector<float>> result);

    ProducerConsumer<std::vector<float>> m_producerConsumer;

    std::vector<float> m_fftBuffer;

    // Lock free way to regularly update a result while another thread is reading it
    std::atomic<std::shared_ptr<std::vector<float>>> m_lastResult;

    std::mutex m_resultMtx;
};