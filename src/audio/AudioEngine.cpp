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
        noise->isOn = preset.chirpNoiseOn.load();
        noise->gain.SetLinear(preset.chirpNoiseVolume.load());
    });


    // Low pass
    std::shared_ptr<AudioProcessor> lpFilter = std::make_shared<LowPassFilter>(
        Frequency(m_preset->chirpLpFilterCutoff.load()),
        m_preset->chirpLpFilterQ.load()
    );
    lpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        BaseFilter *f = dynamic_cast<BaseFilter*>(self);
        f->SetCutoffAndPeaking(Frequency(preset.chirpLpFilterCutoff.load()), preset.chirpLpFilterQ.load());
        f->isOn = preset.chirpLpFilterOn.load();
        f->mix = preset.chirpLpFilterMix.load();
    });
    lpFilter->AddChild(sineOsc);
    lpFilter->AddChild(noiseOsc);


    // High pass
    std::shared_ptr<AudioProcessor> hpFilter = std::make_shared<HighPassFilter>(
        Frequency(m_preset->chirpHpFilterCutoff.load()),
        m_preset->chirpHpFilterQ.load()
    );
    hpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        BaseFilter *f = dynamic_cast<BaseFilter*>(self);
        f->SetCutoffAndPeaking(Frequency(preset.chirpHpFilterCutoff.load()), preset.chirpHpFilterQ.load());
        f->isOn = preset.chirpHpFilterOn.load();
        f->mix = preset.chirpHpFilterMix.load();
    });
    hpFilter->AddChild(lpFilter);


    // Reverb
    std::shared_ptr<AudioProcessor> reverb = std::make_shared<Reverb>();
    reverb->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Reverb *f = dynamic_cast<Reverb *>(self);
        f->SetParams(preset.chirpReverbFeedback.load(), preset.chirpReverbDamp.load(), preset.chirpReverbWet.load());
        f->isOn = preset.chirpReverbOn.load();
    });
    reverb->AddChild(hpFilter);


    // Global mixer
    std::shared_ptr<AudioProcessor> mixer = std::make_shared<Mixer>();
    mixer->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Mixer *m = dynamic_cast<Mixer*>(self);
        m->gain.SetLinear(preset.chirpMasterVolume.load());
    });

    mixer->AddChild(reverb);


    // Set root
    m_chirpRoot = mixer;
}

void AudioEngine::InitSynthAudioProcessorTree() {
    std::shared_ptr<LFO> filterEnv = std::make_shared<Envelope>(0.0f, 0.0f, 0.4f, 0.0f);

    // Osc1
    std::shared_ptr<AudioProcessor> oscA = std::make_shared<Oscillator>(WaveformInfo::Type::Sine);
    oscA->SetCallbackForReadingPreset([filterEnv](AudioProcessor *self, const AudioPreset& preset) {
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
                if (oscA->NoteOn(Frequency(note)) && filterEnv != nullptr) {
                    // If we successfully started to play a new note, restart the cutoff envelope
                    dynamic_cast<Envelope*>(filterEnv.get())->Restart();
                }
            }else {
                oscA->NoteOff(Frequency(note));
            }
        }

        oscA->SetWaveformType(preset.synthOscAWaveform.load());
        oscA->SetEnvelope(Envelope(
            preset.synthOscAAttack.load(),
            preset.synthOscAHold.load(),
            preset.synthOscADec.load(),
            preset.synthOscASus.load()
        ));
        oscA->SetOctave(preset.synthOscAOctave.load());

        oscA->isOn = preset.synthOscAOn.load();
        oscA->gain.SetLinear(preset.synthOscAVolume.load());
        oscA->pan.Set(preset.synthOscAPan.load());
    });

    // Low pass
    std::shared_ptr<AudioProcessor> lpFilter = std::make_shared<LowPassFilter>(
        Frequency(m_preset->synthLpFilterCutoff.load()),
        m_preset->synthLpFilterQ.load()
    );
    lpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        BaseFilter *f = dynamic_cast<BaseFilter*>(self);
        f->SetCutoffAndPeaking(Frequency(preset.synthLpFilterCutoff.load()), preset.synthLpFilterQ.load());
        f->isOn = preset.synthLpFilterOn.load();
        f->mix = preset.synthLpFilterMix.load();
    });
    lpFilter->AddChild(oscA);

    filterEnv->callback = [preset = m_preset](LFO *lfo, AudioProcessor *ap) {
        Envelope *env = dynamic_cast<Envelope*>(lfo);
        env->attack = preset->synthOscALpCutoffAttack.load();
        env->dec = preset->synthOscALpCutoffDec.load();
        LowPassFilter *osc = dynamic_cast<LowPassFilter*>(ap);
        osc->AddCutoffModulation(lfo->GetNextSample() * preset->synthOscALpCutoffAmount.load());
    };
    lpFilter->AddLFO(filterEnv);


    // High pass
    std::shared_ptr<AudioProcessor> hpFilter = std::make_shared<HighPassFilter>(
        Frequency(m_preset->synthHpFilterCutoff.load()),
        m_preset->synthHpFilterQ.load()
    );
    hpFilter->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        BaseFilter *f = dynamic_cast<BaseFilter*>(self);
        f->SetCutoffAndPeaking(Frequency(preset.synthHpFilterCutoff.load()), preset.synthHpFilterQ.load());
        f->isOn = preset.synthHpFilterOn.load();
        f->mix = preset.synthHpFilterMix.load();
    });
    hpFilter->AddChild(lpFilter);


    // Reverb
    std::shared_ptr<AudioProcessor> reverb = std::make_shared<Reverb>();
    reverb->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Reverb *f = dynamic_cast<Reverb *>(self);
        f->SetParams(preset.synthReverbFeedback.load(), preset.synthReverbDamp.load(), preset.synthReverbWet.load());
        f->isOn = preset.synthReverbOn.load();
    });
    reverb->AddChild(hpFilter);


    // Global mixer
    std::shared_ptr<AudioProcessor> mixer = std::make_shared<Mixer>();
    mixer->SetCallbackForReadingPreset([](AudioProcessor *self, const AudioPreset& preset) {
        Mixer *m = dynamic_cast<Mixer*>(self);
        m->gain.SetLinear(preset.synthMasterVolume.load());
    });

    mixer->AddChild(reverb);


    // Set root
    m_synthRoot = mixer;
}