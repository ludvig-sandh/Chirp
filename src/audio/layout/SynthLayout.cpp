// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "layout/SynthLayout.hpp"

#include <cassert>

SynthLayout::SynthLayout()
    : m_oscA(std::make_shared<Oscillator>())
    , m_oscB(std::make_shared<Oscillator>())
    , m_lpFilter(std::make_shared<LowPassFilter>())
    , m_hpFilter(std::make_shared<HighPassFilter>())
    , m_delay(std::make_shared<FeedbackDelay>())
    , m_reverb(std::make_shared<Reverb>())
    , m_mixer(std::make_shared<Mixer>())
    , m_lfo1Periodic(std::make_shared<PeriodicLFO>())
    , m_lfo1Env(std::make_shared<Envelope>())
    , m_lfo1Rnd(std::make_shared<RandomLFO>())
    , m_lfo2Periodic(std::make_shared<PeriodicLFO>())
    , m_lfo2Env(std::make_shared<Envelope>())
    , m_lfo2Rnd(std::make_shared<RandomLFO>())
    , m_filterEnv(std::make_shared<Envelope>(0.0f, 0.0f, 0.0f, 0.0f))
{
    // Now connect all nodes into a graph
    m_lpFilter->AddChild(m_oscA);
    m_lpFilter->AddChild(m_oscB);
    m_hpFilter->AddChild(m_lpFilter);
    m_delay->AddChild(m_hpFilter);
    m_reverb->AddChild(m_delay);
    m_mixer->AddChild(m_reverb);
}

std::shared_ptr<AudioProcessor> SynthLayout::GetRootNode() {
    return m_mixer;
}

void SynthLayout::LoadPreset(AudioPreset& preset) {
    // Map all key inputs to notes and play the ones currently held down
    static std::vector<std::pair<const std::atomic<bool>*, Note>> keySettingPairs = {
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
            bool startedNewNote = m_oscA->NoteOn(Frequency(note));
            m_oscB->NoteOn(Frequency(note));
            if (startedNewNote) {
                m_filterEnv->Restart();
            }
            if (startedNewNote && preset.synthLFO1Mode == LFOConfig::Mode::Envelope) {
                m_lfo1Env->Restart();
            }
            if (startedNewNote && preset.synthLFO2Mode == LFOConfig::Mode::Envelope) {
                m_lfo2Env->Restart();
            }
        }else {
            m_oscA->NoteOff(Frequency(note));
            m_oscB->NoteOff(Frequency(note));
        }
    }

    m_oscA->isOn = preset.synthOscAOn.load();
    m_oscA->gain.SetLinear(preset.synthOscAVolume.load());
    m_oscA->pan.Set(preset.synthOscAPan.load());
    m_oscA->SetWaveformType(preset.synthOscAWaveform.load());
    m_oscA->SetEnvelope(Envelope(
        preset.synthOscAttack.load(),
        preset.synthOscHold.load(),
        preset.synthOscDec.load(),
        preset.synthOscSus.load()
    ));
    m_oscA->SetOctave(preset.synthOscAOctave.load());

    m_oscB->isOn = preset.synthOscBOn.load();
    m_oscB->gain.SetLinear(preset.synthOscBVolume.load());
    m_oscB->pan.Set(preset.synthOscBPan.load());
    m_oscB->SetWaveformType(preset.synthOscBWaveform.load());
    m_oscB->SetEnvelope(Envelope(
        preset.synthOscAttack.load(),
        preset.synthOscHold.load(),
        preset.synthOscDec.load(),
        preset.synthOscSus.load()
    ));
    m_oscB->SetOctave(preset.synthOscBOctave.load());

    m_lpFilter->isOn = preset.synthLpFilterOn.load();
    m_lpFilter->mix = preset.synthLpFilterMix.load();
    m_lpFilter->SetCutoffAndPeaking(Frequency(preset.synthLpFilterCutoff.load()), preset.synthLpFilterQ.load());

    m_hpFilter->isOn = preset.synthHpFilterOn.load();
    m_hpFilter->mix = preset.synthHpFilterMix.load();
    m_hpFilter->SetCutoffAndPeaking(Frequency(preset.synthHpFilterCutoff.load()), preset.synthHpFilterQ.load());

    m_delay->isOn = preset.synthDelayOn.load();
    m_delay->mix = preset.synthDelayMix.load();
    m_delay->SetDelayType(preset.synthDelayType.load());
    m_delay->SetDelayTime(preset.synthDelayTime.load());
    m_delay->SetFeedback(preset.synthDelayFeedback.load());

    m_reverb->isOn = preset.synthReverbOn.load();
    m_reverb->SetParams(preset.synthReverbFeedback.load(), preset.synthReverbDamp.load(), preset.synthReverbWet.load());

    m_mixer->gain.SetLinear(preset.synthMasterVolume.load());

    // Update LFOs
    m_modMatrix.ClearRoutes();

    m_filterEnv->attack = preset.synthOscLpCutoffAttack.load();
    m_filterEnv->dec = preset.synthOscLpCutoffDec.load();
    m_modMatrix.AddRoute(ModulationRoute(m_filterEnv, m_lpFilter, ModulationType::Cutoff, preset.synthOscLpCutoffAmount.load()));

    // Lfo1
    Frequency lfo1Freq = Frequency(preset.synthLFO1Frequency.load());

    m_lfo1Periodic->SetWaveformType(preset.synthLFO1Waveform.load());
    m_lfo1Periodic->SetFrequency(lfo1Freq);
    
    m_lfo1Env->attack = preset.synthLFO1EnvAttack.load();
    m_lfo1Env->hold = preset.synthLFO1EnvHold.load();
    m_lfo1Env->dec = preset.synthLFO1EnvDec.load();
    m_lfo1Env->sus = preset.synthLFO1EnvSus.load();

    m_lfo1Rnd->SetFrequency(lfo1Freq);

    // Lfo2
    Frequency lfo2Freq = Frequency(preset.synthLFO2Frequency.load());

    m_lfo2Periodic->SetWaveformType(preset.synthLFO2Waveform.load());
    m_lfo2Periodic->SetFrequency(lfo2Freq);
    
    m_lfo2Env->attack = preset.synthLFO2EnvAttack.load();
    m_lfo2Env->hold = preset.synthLFO2EnvHold.load();
    m_lfo2Env->dec = preset.synthLFO2EnvDec.load();
    m_lfo2Env->sus = preset.synthLFO2EnvSus.load();

    m_lfo2Rnd->SetFrequency(lfo2Freq);

    // Read modulation matrix from preset
    LFOConfig lfo1Config = ReadLFO1Config(preset);
    LFOConfig lfo2Config = ReadLFO2Config(preset);
    AddModulationRoutesForLfoConfig(lfo1Config);
    AddModulationRoutesForLfoConfig(lfo2Config);
}

void SynthLayout::ApplyAllModulations() {
    m_modMatrix.ApplyModulations();
}

LFOConfig SynthLayout::ReadLFO1Config(AudioPreset& preset) const {
    LFOConfig lfo1Config;
    lfo1Config.on = preset.synthLFO1On.load();
    lfo1Config.mode = preset.synthLFO1Mode.load();
    lfo1Config.destination = preset.synthLFO1Destination.load();
    lfo1Config.amount = preset.synthLFO1Amount.load();
    lfo1Config.envAttack = preset.synthLFO1EnvAttack.load();
    lfo1Config.envHold = preset.synthLFO1EnvHold.load();
    lfo1Config.envDec = preset.synthLFO1EnvDec.load();
    lfo1Config.envSus = preset.synthLFO1EnvSus.load();
    lfo1Config.waveform = preset.synthLFO1Waveform.load();
    lfo1Config.frequency = preset.synthLFO1Frequency.load();
    lfo1Config.lfoNum = 1;
    return lfo1Config;
}

LFOConfig SynthLayout::ReadLFO2Config(AudioPreset& preset) const {
    LFOConfig lfo2Config;
    lfo2Config.on = preset.synthLFO2On.load();
    lfo2Config.mode = preset.synthLFO2Mode.load();
    lfo2Config.destination = preset.synthLFO2Destination.load();
    lfo2Config.amount = preset.synthLFO2Amount.load();
    lfo2Config.envAttack = preset.synthLFO2EnvAttack.load();
    lfo2Config.envHold = preset.synthLFO2EnvHold.load();
    lfo2Config.envDec = preset.synthLFO2EnvDec.load();
    lfo2Config.envSus = preset.synthLFO2EnvSus.load();
    lfo2Config.waveform = preset.synthLFO2Waveform.load();
    lfo2Config.frequency = preset.synthLFO2Frequency.load();
    lfo2Config.lfoNum = 2;
    return lfo2Config;
}

std::shared_ptr<LFO> SynthLayout::GetLFOSourceHelper(LFOConfig::Mode mode, int lfoNum) {
    assert((lfoNum == 1 || lfoNum == 2) && "Only two LFOs have been added. If adding another LFO, update this function.");
    std::shared_ptr<LFO> source;
    switch (mode) {
        case LFOConfig::Mode::Periodic:
            source = lfoNum == 1 ? m_lfo1Periodic : m_lfo2Periodic;
            break;
        case LFOConfig::Mode::Envelope:
            source = lfoNum == 1 ? m_lfo1Env : m_lfo2Env;
            break;
        case LFOConfig::Mode::Random:
            source = lfoNum == 1 ? m_lfo1Rnd : m_lfo2Rnd;
            break;
        default:
            assert(false && "Unknown LFO config mode");
            break;
    }
    return source;
}

void SynthLayout::AddModulationRoutesForLfoConfig(const LFOConfig& config) {
    if (!config.on) {
        return; // Don't add a disabled modulation
    }

    std::shared_ptr<LFO> source = GetLFOSourceHelper(config.mode, config.lfoNum);
    
    // All selectable routes in GUI and add the corresponding route
    switch (config.destination) {
        case LFOConfig::Destination::OscAVolume:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscA, ModulationType::Volume, config.amount));
            break;
        case LFOConfig::Destination::OscAPan:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscA, ModulationType::Pan, config.amount));
            break;
        case LFOConfig::Destination::OscAPitch:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscA, ModulationType::Pitch, config.amount));
            break;
        case LFOConfig::Destination::OscBVolume:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscB, ModulationType::Volume, config.amount));
            break;
        case LFOConfig::Destination::OscBPan:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscB, ModulationType::Pan, config.amount));
            break;
        case LFOConfig::Destination::OscBPitch:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscB, ModulationType::Pitch, config.amount));
            break;
        case LFOConfig::Destination::OscABVolume:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscA, ModulationType::Volume, config.amount));
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscB, ModulationType::Volume, config.amount));
            break;
        case LFOConfig::Destination::OscABPan:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscA, ModulationType::Pan, config.amount));
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscB, ModulationType::Pan, config.amount));
            break;
        case LFOConfig::Destination::OscABPitch:
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscA, ModulationType::Pitch, config.amount));
            m_modMatrix.AddRoute(ModulationRoute(source, m_oscB, ModulationType::Pitch, config.amount));
            break;
        case LFOConfig::Destination::LPCutoff:
            m_modMatrix.AddRoute(ModulationRoute(source, m_lpFilter, ModulationType::Cutoff, config.amount));
            break;
        case LFOConfig::Destination::LPPeaking:
            m_modMatrix.AddRoute(ModulationRoute(source, m_lpFilter, ModulationType::Peaking, config.amount));
            break;
        case LFOConfig::Destination::HPCutoff:
            m_modMatrix.AddRoute(ModulationRoute(source, m_hpFilter, ModulationType::Cutoff, config.amount));
            break;
        case LFOConfig::Destination::HPPeaking:
            m_modMatrix.AddRoute(ModulationRoute(source, m_lpFilter, ModulationType::Peaking, config.amount));
            break;
        default:
            assert(false && "Unknown LFO destination");
            break;
    }
}