// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <unordered_set>

#include "portaudio.h"
#include "audio/preset/AudioPreset.hpp"
#include "fft/FFTComputer.hpp"
#include "audio/engine/AudioBuffer.hpp"
#include "audio/engine/AudioBackend.hpp"
#include "audio/layout/SynthLayout.hpp"

namespace Audio::Engine {
    namespace Constants {
        inline constexpr int SAMPLE_RATE = 44100;
    }

class AudioEngine {
public:
    AudioEngine(std::shared_ptr<Audio::Preset::AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer);

    // Recurse from the root of the tree
    AudioBuffer ProcessBuffer(int numFrames);

    // Starts the audio engine
    void Start(std::atomic<bool>& running);

private:
    // RAII class for taking care of portaudio init/deinit
    class ScopedPaHandler {
    public:
        ScopedPaHandler() : m_result(Pa_Initialize()) {}
        ~ScopedPaHandler() {
            if (m_result == paNoError) {
                Pa_Terminate();
            }
        }

        PaError Result() const { return m_result; }

    private:
        PaError m_result;
    };

    std::shared_ptr<Audio::Preset::AudioPreset> m_preset;
    std::shared_ptr<FFTComputer> m_fftComputer;
    AudioBackend m_backend;
    Audio::Layout::SynthLayout m_synthLayout;
};

} // namespace Audio::Engine
