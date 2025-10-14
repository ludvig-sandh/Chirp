#include "FFTComputer.hpp"

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
            std::cout << "-";
            continue;
        }

        // Extend fft buffer with the new audio
        m_fftBuffer.reserve(m_fftBuffer.size() + distance(audioToExtend->begin(), audioToExtend->end()));
        m_fftBuffer.insert(m_fftBuffer.end(), audioToExtend->begin(), audioToExtend->end());

        if (m_fftBuffer.size() >= s_fftSize) {
            std::shared_ptr<std::vector<float>> fft = FFTComputer::FFTHelper(m_fftBuffer);
            m_fftBuffer.clear(); // Clear window
            StoreNewResult(fft);
        }
    }

    std::cout << "#\n";
    m_producerConsumer.Close();
}

std::shared_ptr<std::vector<float>> FFTComputer::FFTHelper(std::vector<float>& window) {
    assert(window.size() >= s_fftSize && "Programming error: to compute the fft we need large enough buffer.");

    // Temporary: compute audio volume rather than FFT for now
    auto maxIt = std::max_element(window.begin(), window.end());
    float maxVol = 0.0f;

    if (maxIt != window.end()) {
        maxVol = *maxIt;
    }
    
    int volLevels = static_cast<int>(maxVol * 256);
    std::shared_ptr<std::vector<float>> result = std::make_shared<std::vector<float>>(256);
    for (int i = 0; i < std::min(volLevels, 256); i++) {
        (*result)[i] = 1.0;
    }

    return result;
}

void FFTComputer::FinishedProducing() {
    m_producerConsumer.Close();
}