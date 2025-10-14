#include "FFTComputer.hpp"
#include "FFTHelper.hpp"

#include <cassert>
#include <iostream>

std::shared_ptr<std::vector<float>> FFTComputer::GetLastResult() const {
    return m_lastResult.load(std::memory_order_acquire);
}

void FFTComputer::StoreNewResult(std::shared_ptr<std::vector<float>> result) {
    m_lastResult.store(result, std::memory_order_release);
}

void FFTComputer::ProvideAudioBuffer(const AudioBuffer& buffer) {
    // Copy over the output buffer into a vector we can "produce"
    unsigned long outputLength = buffer.framesPerBuffer;
    std::unique_ptr<std::vector<float>> output = std::make_unique<std::vector<float>>(outputLength);
    float *read = static_cast<float*>(buffer.outputBuffer);
    for (size_t i = 0; i < outputLength; i++) {
        float left = *read++;
        float right = *read++;
        (*output).at(i) = (left + right) / 2.0; // Average across channels
    }

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

        // Extend fft buffer with the new audio
        m_fftBuffer.reserve(m_fftBuffer.size() + distance(audioToExtend->begin(), audioToExtend->end()));
        m_fftBuffer.insert(m_fftBuffer.end(), audioToExtend->begin(), audioToExtend->end());

        if (m_fftBuffer.size() > 0) {
            std::shared_ptr<std::vector<float>> fft_magnitude = FFTHelper::ComputeFFTMagnitude(m_fftBuffer);
            m_fftBuffer.clear(); // Clear window
            StoreNewResult(fft_magnitude);
        }
    }

    m_producerConsumer.Close();
}

void FFTComputer::FinishedProducing() {
    m_producerConsumer.Close();
}