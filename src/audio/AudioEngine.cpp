#include <stdio.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <thread>
#include <unordered_set>

#include "portaudio.h"
#include "AudioEngine.hpp"
#include "AudioBackend.hpp"
#include "Frequency.hpp"
#include "AudioPreset.hpp"
#include "Oscillator.hpp"
#include "Waveform.hpp"
#include "RandomLFO.hpp"
#include "Mixer.hpp"
#include "LowPassFilter.hpp"

AudioEngine::AudioEngine(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer)
    : m_preset(preset)
    , m_fftComputer(fftComputer)
    , m_backend(this)
{
    InitAudioProcessorTree();
}

// Recurse from the root of the tree
void AudioEngine::ProcessBuffer(AudioBuffer &buffer) {
    if (m_rootProcessor) {
        m_rootProcessor->Process(buffer, *m_preset.get());

        // Temporary fix for reducing the way too loud volume
        for (unsigned long i = 0; i < buffer.framesPerBuffer * 2; i++) {
            ((float*)buffer.outputBuffer)[i] *= 0.5;
        }

        // Send buffer to FFT thread
        m_fftComputer->ProvideAudioBuffer(buffer);
    }
}

void AudioEngine::Start(std::atomic<bool>& running) {
    ScopedPaHandler paInit;
    if (paInit.result() != paNoError) {
        std::cerr << "An error occurred while using the portaudio stream\n";
        std::cerr << "Error number: %d\n" << paInit.result();
        std::cerr << "Error message: %s\n" << Pa_GetErrorText( paInit.result() );
    }

    if (m_backend.open(Pa_GetDefaultOutputDevice())) {
        if (m_backend.start()) {
            while (running.load()) {
                Pa_Sleep(50); // Sleep 50ms
            }
            m_backend.stop();
        }
        
        m_backend.close();
        printf("Audio stopped.\n");
    }
    m_fftComputer->FinishedProducing();
}

void AudioEngine::InitAudioProcessorTree() {
    // TODO: Read configuration from file?
    std::shared_ptr<Waveform> sine = std::make_shared<Sine>();
    std::shared_ptr<Waveform> saw = std::make_shared<Saw>();
    std::shared_ptr<Waveform> sq = std::make_shared<Square>();
    std::shared_ptr<Waveform> noise = std::make_shared<WhiteNoise>();
    std::shared_ptr<AudioProcessor> sineOsc = std::make_shared<Oscillator>(saw, Frequency(200));
    // std::shared_ptr<AudioProcessor> sineOsc = std::make_shared<Oscillator>(sine, Frequency(2220));
    std::shared_ptr<AudioProcessor> sawOsc = std::make_shared<Oscillator>(saw, Frequency(555));
    std::shared_ptr<AudioProcessor> noiseOsc = std::make_shared<Oscillator>(noise, Frequency(555));
    std::shared_ptr<AudioProcessor> mixer = std::make_shared<Mixer>();

    std::shared_ptr<LFO> rnd = std::make_shared<RandomLFO>(Frequency(16));
    rnd->callback = [](LFO *lfo, AudioProcessor *ap) {
        Oscillator *osc = dynamic_cast<Oscillator *>(ap);
        osc->frequency.AddPitchModulation(lfo->GetNextSample() * 64);
    };

    // std::shared_ptr<LFO> vibrato = std::make_shared<Oscillator>(saw, Frequency(62));
    std::shared_ptr<LFO> vibrato = std::make_shared<Oscillator>(saw, Frequency(0.5));
    vibrato->callback = [](LFO *lfo, AudioProcessor *ap) {
        Oscillator *osc = dynamic_cast<Oscillator *>(ap);
        osc->frequency.AddPitchModulation(lfo->GetNextSample() - 0.5);
        // osc->frequency.AddPitchModulation(12 * 10 * lfo->GetNextSample());
    };

    // sineOsc->AddLFO(rnd); // Connect the rnd LFO to the sine oscillator
    // sineOsc->AddLFO(vibrato);

    mixer->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Mixer *m = dynamic_cast<Mixer*>(self);
        m->gain.Set(preset.masterVolume.load());
        m->pan.Set(preset.masterPan.load());
    });

    mixer->AddChild(sineOsc);
    mixer->AddChild(noiseOsc);
    dynamic_cast<Oscillator *>(noiseOsc.get())->gain.Set(0.2f);

    std::shared_ptr<AudioProcessor> lpFilter = std::make_shared<LowPassFilter>(
        m_preset->lpFilterCutoff.load(),
        m_preset->lpFilterSteepness.load()
    );
    lpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        LowPassFilter *f = dynamic_cast<LowPassFilter*>(self);
        f->SetCutoff(preset.lpFilterCutoff.load());
        f->SetSteepness(preset.lpFilterSteepness.load());
        f->isOn = preset.lpFilterOn.load();
        f->mix = preset.lpFilterMix.load();
    });

    lpFilter->AddChild(mixer);

    // Set root
    m_rootProcessor = lpFilter;
}