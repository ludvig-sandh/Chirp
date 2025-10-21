#pragma once

#include <memory>
#include <unordered_set>

#include "AudioBackend.hpp"
#include "AudioPreset.hpp"
#include "AudioProcessor.hpp"
#include "FFTComputer.hpp"

#define SAMPLE_RATE   (44100)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)

class AudioEngine {
public:
    AudioEngine(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer);

    // Recurse from the root of the tree
    AudioBuffer ProcessBuffer(size_t numFrames);

    void Start(std::atomic<bool>& running);
private:
    void InitChirpAudioProcessorTree();
    void InitSynthAudioProcessorTree();

    std::shared_ptr<AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    AudioBackend m_backend;
    std::shared_ptr<AudioProcessor> m_chirpRoot;
    std::shared_ptr<AudioProcessor> m_synthRoot;
};