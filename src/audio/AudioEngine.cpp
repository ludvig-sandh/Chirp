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
#include "BaseFilter.hpp"
#include "LowPassFilter.hpp"
#include "HighPassFilter.hpp"
#include "Reverb.hpp"

AudioEngine::AudioEngine(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer)
    : m_preset(preset)
    , m_fftComputer(fftComputer)
    , m_backend(this)
{
    InitChirpAudioProcessorTree();
    InitSynthAudioProcessorTree();
}

// Recurse from the root of the tree
AudioBuffer AudioEngine::ProcessBuffer(size_t numFrames) {
    std::shared_ptr<AudioProcessor> rootProcessor;
    if (m_preset->appMode.load() == AppMode::Chirp) {
        rootProcessor = m_chirpRoot;
    }else {
        rootProcessor = m_synthRoot;
    }

    if (rootProcessor) {
        rootProcessor->ClearVisited();

        AudioBuffer result = rootProcessor->Process(numFrames, *m_preset.get());

        // Send buffer to FFT thread
        m_fftComputer->ProvideAudioBuffer(result);
        
        return result;
    }

    return AudioBuffer(numFrames);
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

void AudioEngine::InitChirpAudioProcessorTree() {
    // Chirp
    std::shared_ptr<AudioProcessor> sineOsc = std::make_shared<Oscillator>(WaveformInfo::Type::Sine);
    dynamic_cast<Oscillator*>(sineOsc.get())->NoteOn(Frequency(2220.0f));
    sineOsc->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Oscillator *sine = dynamic_cast<Oscillator*>(self);
        sine->isOn = preset.chirpOn.load();
        sine->gain.SetLinear(preset.chirpVolume.load());
        sine->pan.Set(preset.chirpPan.load());
    });

    std::shared_ptr<LFO> rnd = std::make_shared<RandomLFO>(Frequency(16));
    rnd->callback = [](LFO *lfo, AudioProcessor *ap) {
        Oscillator *osc = dynamic_cast<Oscillator *>(ap);
        osc->AddPitchModulation(lfo->GetNextSample() * 64);
    };

    std::shared_ptr<LFO> vibrato = std::make_shared<Oscillator>(WaveformInfo::Type::Saw);
    dynamic_cast<Oscillator*>(vibrato.get())->NoteOn(Frequency(62.0f));
    vibrato->callback = [](LFO *lfo, AudioProcessor *ap) {
        Oscillator *osc = dynamic_cast<Oscillator *>(ap);
        osc->AddPitchModulation(lfo->GetNextSample() - 0.5);
    };

    sineOsc->AddLFO(rnd); // Connect the rnd LFO to the sine oscillator
    sineOsc->AddLFO(vibrato);

    
    // Noise
    std::shared_ptr<AudioProcessor> noiseOsc = std::make_shared<Oscillator>(WaveformInfo::Type::WhiteNoise);
    dynamic_cast<Oscillator*>(noiseOsc.get())->NoteOn(Frequency(555.0f));
    noiseOsc->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Oscillator *noise = dynamic_cast<Oscillator*>(self);
        noise->isOn = preset.noiseOn.load();
        noise->gain.SetLinear(preset.noiseVolume.load());
    });


    // Low pass
    std::shared_ptr<AudioProcessor> lpFilter = std::make_shared<LowPassFilter>(
        m_preset->lpFilterCutoff.load(),
        m_preset->lpFilterQ.load()
    );
    lpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        BaseFilter *f = dynamic_cast<BaseFilter*>(self);
        f->SetCutoffAndPeaking(preset.lpFilterCutoff.load(), preset.lpFilterQ.load());
        f->isOn = preset.lpFilterOn.load();
        f->mix = preset.lpFilterMix.load();
    });
    lpFilter->AddChild(sineOsc);
    lpFilter->AddChild(noiseOsc);


    // High pass
    std::shared_ptr<AudioProcessor> hpFilter = std::make_shared<HighPassFilter>(
        m_preset->hpFilterCutoff.load(),
        m_preset->hpFilterQ.load()
    );
    hpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        BaseFilter *f = dynamic_cast<BaseFilter*>(self);
        f->SetCutoffAndPeaking(preset.hpFilterCutoff.load(), preset.hpFilterQ.load());
        f->isOn = preset.hpFilterOn.load();
        f->mix = preset.hpFilterMix.load();
    });
    hpFilter->AddChild(lpFilter);


    // Reverb
    std::shared_ptr<AudioProcessor> reverb = std::make_shared<Reverb>();
    reverb->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Reverb *f = dynamic_cast<Reverb *>(self);
        f->SetParams(preset.reverbFeedback.load(), preset.reverbDamp.load(), preset.reverbWet.load());
        f->isOn = preset.reverbOn.load();
    });
    reverb->AddChild(hpFilter);


    // Global mixer
    std::shared_ptr<AudioProcessor> mixer = std::make_shared<Mixer>();
    mixer->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Mixer *m = dynamic_cast<Mixer*>(self);
        m->gain.SetLinear(preset.masterVolume.load());
    });

    mixer->AddChild(reverb);


    // Set root
    m_chirpRoot = mixer;
}

void AudioEngine::InitSynthAudioProcessorTree() {
    // Osc1
    std::shared_ptr<AudioProcessor> oscA = std::make_shared<Oscillator>(WaveformInfo::Type::Sine);
    oscA->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Oscillator *oscA = dynamic_cast<Oscillator*>(self);

        // Map all key inputs to notes and play the ones currently held down
        std::vector<std::pair<const std::atomic<bool>*, Note>> keySettingPairs = {
            { &preset.noteA5, Note(Key::A, 5) },
            { &preset.noteAs5, Note(Key::As, 5) },
            { &preset.noteB5, Note(Key::B, 5) },
            { &preset.noteC5, Note(Key::C, 5) },
            { &preset.noteCs5, Note(Key::Cs, 5) },
            { &preset.noteD5, Note(Key::D, 5) },
            { &preset.noteDs5, Note(Key::Ds, 5) },
            { &preset.noteE5, Note(Key::E, 5) },
            { &preset.noteF5, Note(Key::F, 5) },
            { &preset.noteFs5, Note(Key::Fs, 5) },
            { &preset.noteG5, Note(Key::G, 5) },
            { &preset.noteGs5, Note(Key::Gs, 5) }
        };

        for (auto& [keySettingPtr, note] : keySettingPairs) {
            if (keySettingPtr->load()) {
                oscA->NoteOn(Frequency(note));
            }else {
                oscA->NoteOff(Frequency(note));
            }
        }

        oscA->SetWaveformType(preset.oscAWaveform.load());
        oscA->SetEnvelope(Envelope(
            preset.oscAAttack.load(),
            preset.oscAHold.load(),
            preset.oscADec.load(),
            preset.oscASus.load()
        ));
        oscA->isOn = preset.oscAOn.load();
        oscA->gain.SetLinear(preset.oscAVolume.load());
        oscA->pan.Set(preset.oscAPan.load());
    });

    // Low pass
    std::shared_ptr<AudioProcessor> lpFilter = std::make_shared<LowPassFilter>(
        m_preset->lpFilterCutoff.load(),
        m_preset->lpFilterQ.load()
    );
    lpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        BaseFilter *f = dynamic_cast<BaseFilter*>(self);
        f->SetCutoffAndPeaking(preset.lpFilterCutoff.load(), preset.lpFilterQ.load());
        f->isOn = preset.lpFilterOn.load();
        f->mix = preset.lpFilterMix.load();
    });
    lpFilter->AddChild(oscA);


    // High pass
    std::shared_ptr<AudioProcessor> hpFilter = std::make_shared<HighPassFilter>(
        m_preset->hpFilterCutoff.load(),
        m_preset->hpFilterQ.load()
    );
    hpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        BaseFilter *f = dynamic_cast<BaseFilter*>(self);
        f->SetCutoffAndPeaking(preset.hpFilterCutoff.load(), preset.hpFilterQ.load());
        f->isOn = preset.hpFilterOn.load();
        f->mix = preset.hpFilterMix.load();
    });
    hpFilter->AddChild(lpFilter);


    // Reverb
    std::shared_ptr<AudioProcessor> reverb = std::make_shared<Reverb>();
    reverb->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Reverb *f = dynamic_cast<Reverb *>(self);
        f->SetParams(preset.reverbFeedback.load(), preset.reverbDamp.load(), preset.reverbWet.load());
        f->isOn = preset.reverbOn.load();
    });
    reverb->AddChild(hpFilter);


    // Global mixer
    std::shared_ptr<AudioProcessor> mixer = std::make_shared<Mixer>();
    mixer->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Mixer *m = dynamic_cast<Mixer*>(self);
        m->gain.SetLinear(preset.masterVolume.load());
    });

    mixer->AddChild(reverb);


    // Set root
    m_synthRoot = mixer;
}