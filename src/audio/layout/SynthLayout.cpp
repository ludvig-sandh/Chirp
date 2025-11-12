// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/layout/SynthLayout.hpp"
#include "audio/effects/dsp/Time.hpp"
#include "gui/windows/KeyboardWindow.hpp"
#include "util/NormalizedFloat.hpp"
#include <cassert>

namespace Audio::Layout {

SynthLayout::SynthLayout()
    : m_oscA(std::make_shared<Audio::Generator::Oscillator>())
    , m_oscB(std::make_shared<Audio::Generator::Oscillator>())
    , m_lpFilter(std::make_shared<Audio::Effects::LowPassFilter>())
    , m_hpFilter(std::make_shared<Audio::Effects::HighPassFilter>())
    , m_delay(std::make_shared<Audio::Effects::FeedbackDelay>())
    , m_reverb(std::make_shared<Audio::Effects::Reverb>())
    , m_mixer(std::make_shared<Audio::Effects::Mixer>())
    , m_lfo1Periodic(std::make_shared<Audio::Modulation::PeriodicLFO>())
    , m_lfo1Env(std::make_shared<Audio::Modulation::Envelope>())
    , m_lfo1Rnd(std::make_shared<Audio::Modulation::RandomLFO>())
    , m_lfo2Periodic(std::make_shared<Audio::Modulation::PeriodicLFO>())
    , m_lfo2Env(std::make_shared<Audio::Modulation::Envelope>())
    , m_lfo2Rnd(std::make_shared<Audio::Modulation::RandomLFO>())
    , m_filterEnv(std::make_shared<Audio::Modulation::Envelope>(
        Audio::Effects::DSP::Seconds(0.0f),
        Audio::Effects::DSP::Seconds(0.0f),
        Audio::Effects::DSP::Seconds(0.0f),
        Util::NormalizedFloat(0.0f),
        Audio::Effects::DSP::Seconds(0.0f)))
{
    // Now connect all nodes into a graph
    m_lpFilter->AddChild(m_oscA);
    m_lpFilter->AddChild(m_oscB);
    m_hpFilter->AddChild(m_lpFilter);
    m_delay->AddChild(m_hpFilter);
    m_reverb->AddChild(m_delay);
    m_mixer->AddChild(m_reverb);
}

std::shared_ptr<Audio::Engine::AudioProcessorNode> SynthLayout::GetRootNode() {
    return m_mixer;
}

void SynthLayout::LoadPreset(Audio::Preset::AudioPreset& preset) {
    Audio::Core::Note note = GUI::Window::KeyboardWindow::FIRST_NOTE;
    for (auto& isPressed : preset.noteStates) {
        if (isPressed.load()) {
            if (m_pressedNotes.find(note) == m_pressedNotes.end()) {
                m_pressedNotes.insert(note);

                // New note pressed
                if (m_oscA->isOn) {
                    m_oscA->NoteOn(note);
                }
                if (m_oscB->isOn) {
                    m_oscB->NoteOn(note);
                }
                m_filterEnv->Restart();
                m_lfo1Env->Restart();
                m_lfo2Env->Restart();
            }
        }else {
            if (m_pressedNotes.find(note) != m_pressedNotes.end()) {
                m_pressedNotes.erase(note);

                // Note released
                m_oscA->NoteOff(note);
                m_oscB->NoteOff(note);
            }
        }
        
        ++note;
    }

    Audio::Modulation::Envelope volEnvelope = Audio::Modulation::Envelope(
        Audio::Effects::DSP::Seconds(preset.synthOscAttack.load()),
        Audio::Effects::DSP::Seconds(preset.synthOscHold.load()),
        Audio::Effects::DSP::Seconds(preset.synthOscDec.load()),
        Util::NormalizedFloat(preset.synthOscSus.load()),
        Audio::Effects::DSP::Seconds(preset.synthOscRel.load())
    );

    m_oscA->isOn = preset.synthOscAOn.load();
    m_oscA->gain.SetLinear(preset.synthOscAVolume.load());
    m_oscA->pan.Set(preset.synthOscAPan.load());
    m_oscA->SetWaveformType(preset.synthOscAWaveform.load());
    m_oscA->SetEnvelope(volEnvelope);
    m_oscA->SetOctave(preset.synthOscAOctave.load());

    m_oscB->isOn = preset.synthOscBOn.load();
    m_oscB->gain.SetLinear(preset.synthOscBVolume.load());
    m_oscB->pan.Set(preset.synthOscBPan.load());
    m_oscB->SetWaveformType(preset.synthOscBWaveform.load());
    m_oscB->SetEnvelope(volEnvelope);
    m_oscB->SetOctave(preset.synthOscBOctave.load());

    m_lpFilter->isOn = preset.synthLpFilterOn.load();
    m_lpFilter->mix = preset.synthLpFilterMix.load();
    m_lpFilter->SetCutoffAndPeaking(Audio::Core::Frequency(preset.synthLpFilterCutoff.load()), preset.synthLpFilterQ.load());

    m_hpFilter->isOn = preset.synthHpFilterOn.load();
    m_hpFilter->mix = preset.synthHpFilterMix.load();
    m_hpFilter->SetCutoffAndPeaking(Audio::Core::Frequency(preset.synthHpFilterCutoff.load()), preset.synthHpFilterQ.load());

    m_delay->isOn = preset.synthDelayOn.load();
    m_delay->mix = preset.synthDelayMix.load();
    m_delay->SetDelayType(preset.synthDelayType.load());
    m_delay->SetDelayTime(Audio::Effects::DSP::Seconds(preset.synthDelayTime.load()));
    m_delay->SetFeedback(preset.synthDelayFeedback.load());

    m_reverb->isOn = preset.synthReverbOn.load();
    m_reverb->SetParams(preset.synthReverbFeedback.load(), preset.synthReverbDamp.load(), preset.synthReverbWet.load());

    m_mixer->gain.SetLinear(preset.synthMasterVolume.load());

    // Update LFOs
    m_modMatrix.ClearRoutes();

    m_filterEnv->attack = Audio::Effects::DSP::Seconds(preset.synthOscLpCutoffAttack.load());
    m_filterEnv->decay = Audio::Effects::DSP::Seconds(preset.synthOscLpCutoffDec.load());
    m_modMatrix.AddRoute(Audio::Modulation::Route(m_filterEnv, m_lpFilter, Audio::Modulation::Type::Cutoff, preset.synthOscLpCutoffAmount.load()));

    // Lfo1
    Audio::Core::Frequency lfo1Freq{preset.synthLFO1Frequency.load()};

    m_lfo1Periodic->SetWaveformType(preset.synthLFO1Waveform.load());
    m_lfo1Periodic->SetFrequency(lfo1Freq);
    
    m_lfo1Env->attack = Audio::Effects::DSP::Seconds(preset.synthLFO1EnvAttack.load());
    m_lfo1Env->hold = Audio::Effects::DSP::Seconds(preset.synthLFO1EnvHold.load());
    m_lfo1Env->decay = Audio::Effects::DSP::Seconds(preset.synthLFO1EnvDec.load());
    m_lfo1Env->sustain = Util::NormalizedFloat(preset.synthLFO1EnvSus.load());

    m_lfo1Rnd->SetFrequency(lfo1Freq);

    // Lfo2
    Audio::Core::Frequency lfo2Freq{preset.synthLFO2Frequency.load()};

    m_lfo2Periodic->SetWaveformType(preset.synthLFO2Waveform.load());
    m_lfo2Periodic->SetFrequency(lfo2Freq);
    
    m_lfo2Env->attack = Audio::Effects::DSP::Seconds(preset.synthLFO2EnvAttack.load());
    m_lfo2Env->hold = Audio::Effects::DSP::Seconds(preset.synthLFO2EnvHold.load());
    m_lfo2Env->decay = Audio::Effects::DSP::Seconds(preset.synthLFO2EnvDec.load());
    m_lfo2Env->sustain = Util::NormalizedFloat(preset.synthLFO2EnvSus.load());

    m_lfo2Rnd->SetFrequency(lfo2Freq);

    // Read modulation matrix from preset
    Audio::Modulation::LFOConfig lfo1Config = ReadLFO1Config(preset);
    Audio::Modulation::LFOConfig lfo2Config = ReadLFO2Config(preset);
    AddModulationRoutesForLfoConfig(lfo1Config);
    AddModulationRoutesForLfoConfig(lfo2Config);
}

void SynthLayout::ApplyAllModulations() {
    m_modMatrix.ApplyModulations();
}

Audio::Modulation::LFOConfig SynthLayout::ReadLFO1Config(Audio::Preset::AudioPreset& preset) const {
    Audio::Modulation::LFOConfig lfo1Config;
    lfo1Config.on = preset.synthLFO1On.load();
    lfo1Config.mode = preset.synthLFO1Mode.load();
    lfo1Config.destination = preset.synthLFO1Destination.load();
    lfo1Config.amount = preset.synthLFO1Amount.load();
    lfo1Config.lfoNum = 1;
    return lfo1Config;
}

Audio::Modulation::LFOConfig SynthLayout::ReadLFO2Config(Audio::Preset::AudioPreset& preset) const {
    Audio::Modulation::LFOConfig lfo2Config;
    lfo2Config.on = preset.synthLFO2On.load();
    lfo2Config.mode = preset.synthLFO2Mode.load();
    lfo2Config.destination = preset.synthLFO2Destination.load();
    lfo2Config.amount = preset.synthLFO2Amount.load();
    lfo2Config.lfoNum = 2;
    return lfo2Config;
}

std::shared_ptr<Audio::Modulation::LFO> SynthLayout::GetLFOSourceHelper(Audio::Modulation::LFOConfig::Mode mode, int lfoNum) {
    assert((lfoNum == 1 || lfoNum == 2) && "Only two LFOs have been added. If adding another LFO, update this function.");
    std::shared_ptr<Audio::Modulation::LFO> source;
    switch (mode) {
        case Audio::Modulation::LFOConfig::Mode::Periodic:
            source = lfoNum == 1 ? m_lfo1Periodic : m_lfo2Periodic;
            break;
        case Audio::Modulation::LFOConfig::Mode::Envelope:
            source = lfoNum == 1 ? m_lfo1Env : m_lfo2Env;
            break;
        case Audio::Modulation::LFOConfig::Mode::Random:
            source = lfoNum == 1 ? m_lfo1Rnd : m_lfo2Rnd;
            break;
        default:
            assert(false && "Unknown LFO config mode");
            break;
    }
    return source;
}

void SynthLayout::AddModulationRoutesForLfoConfig(const Audio::Modulation::LFOConfig& config) {
    if (!config.on) {
        return; // Don't add a disabled modulation
    }

    std::shared_ptr<Audio::Modulation::LFO> source = GetLFOSourceHelper(config.mode, config.lfoNum);
    
    // All selectable routes in GUI and add the corresponding route
    switch (config.destination) {
        case Audio::Modulation::LFOConfig::Destination::OscAVolume:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscA, Audio::Modulation::Type::Volume, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::OscAPan:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscA, Audio::Modulation::Type::Pan, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::OscAPitch:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscA, Audio::Modulation::Type::Pitch, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::OscBVolume:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscB, Audio::Modulation::Type::Volume, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::OscBPan:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscB, Audio::Modulation::Type::Pan, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::OscBPitch:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscB, Audio::Modulation::Type::Pitch, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::OscABVolume:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscA, Audio::Modulation::Type::Volume, config.amount));
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscB, Audio::Modulation::Type::Volume, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::OscABPan:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscA, Audio::Modulation::Type::Pan, config.amount));
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscB, Audio::Modulation::Type::Pan, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::OscABPitch:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscA, Audio::Modulation::Type::Pitch, config.amount));
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_oscB, Audio::Modulation::Type::Pitch, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::LPCutoff:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_lpFilter, Audio::Modulation::Type::Cutoff, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::LPPeaking:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_lpFilter, Audio::Modulation::Type::Peaking, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::HPCutoff:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_hpFilter, Audio::Modulation::Type::Cutoff, config.amount));
            break;
        case Audio::Modulation::LFOConfig::Destination::HPPeaking:
            m_modMatrix.AddRoute(Audio::Modulation::Route(source, m_lpFilter, Audio::Modulation::Type::Peaking, config.amount));
            break;
        default:
            assert(false && "Unknown LFO destination");
            break;
    }
}

} // namespace Audio::Layout
