#pragma once

#include <memory>
#include <unordered_set>

#include "AudioBackend.hpp"
#include "AudioPreset.hpp"
#include "AudioProcessor.hpp"
#include "FFTComputer.hpp"

#define NUM_SECONDS   (10)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (512)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)

class AudioEngine {
public:
    AudioEngine(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer);

    // Recurse from the root of the tree
    void ProcessBuffer(const AudioBuffer &buffer);

    void Start(std::atomic<bool>& running);
private:
    void InitAudioProcessorTree();

    std::shared_ptr<AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    AudioBackend m_backend;
    std::shared_ptr<AudioProcessor> m_rootProcessor;
};