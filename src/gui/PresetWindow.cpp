// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/PresetWindow.hpp"

#include "gui/Spectrogram.hpp"
#include "gui/LevelsDisplay.hpp"
#include "gui/Keyboard.hpp"
#include "gui/GlobalSettingsWindow.hpp"
#include "gui/PresetLoaderWindow.hpp"
#include "audio/engine/AudioBackend.hpp"
#include "audio/core/Waveform.hpp"
#include "audio/effects/dsp/FeedbackDelayLine.hpp"
#include "audio/effects/dsp/FeedbackDelayInfo.hpp"
#include "audio/modulation/LFO.hpp"
#include "audio/core/Frequency.hpp"

void PresetWindow::Render(float framerate) const {
    ConfigureWindow();

    DrawOscillatorA();
    DrawOscillatorB();
    DrawVolumeEnvelope();
    DrawLowPassFilter();
    DrawHighPassFilter();
    DrawFeedbackDelay();
    DrawReverb();
    DrawLFO1();
    DrawLFO2();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    ImGui::End();
}

void PresetWindow::ConfigureWindow() const {
    // Get viewport (the main window area)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    const float x = viewport->Pos.x;
    const float y = viewport->Pos.y + GlobalSettingsWindow::WINDOW_HEIGHT + PresetLoaderWindow::WINDOW_HEIGHT;
    ImGui::SetNextWindowPos(
        ImVec2(x, y),
        ImGuiCond_Always
    );

    const float windowWidth = viewport->Size.x - Spectrogram::WINDOW_WIDTH - LevelsDisplay::WINDOW_WIDTH;
    const float windowHeight = viewport->Size.y - Keyboard::WINDOW_HEIGHT - GlobalSettingsWindow::WINDOW_HEIGHT - PresetLoaderWindow::WINDOW_HEIGHT;
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin("audio/preset control", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse);
}

void PresetWindow::DrawOscillatorA() const {
    ImGui::SeparatorText("Oscillator A");
    
    bool oscAOnTemp = m_preset->synthOscAOn.load();
    ImGui::Checkbox("On##A", &oscAOnTemp);
    m_preset->synthOscAOn.store(oscAOnTemp);
    
    // --- Waveform dropdown ---
    WaveformInfo::Type waveformATemp = m_preset->synthOscAWaveform.load();

    // ImGui::Combo returns true if the selection changed
    if (ImGui::BeginCombo("Waveform##A", WaveformInfo::Names[static_cast<int>(waveformATemp)])) {
        for (int n = 0; n < IM_ARRAYSIZE(WaveformInfo::Names); n++) {
            bool isSelected = (static_cast<int>(waveformATemp) == n);
            if (ImGui::Selectable(WaveformInfo::Names[n], isSelected)) {
                waveformATemp = static_cast<WaveformInfo::Type>(n);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    m_preset->synthOscAWaveform.store(waveformATemp);
    // ---

    float oscAVolumeTemp = m_preset->synthOscAVolume.load();
    ImGui::SliderFloat("Volume##A", &oscAVolumeTemp, 0.0f, 1.0f);
    m_preset->synthOscAVolume.store(oscAVolumeTemp);

    float oscAPanTemp = m_preset->synthOscAPan.load();
    ImGui::SliderFloat("Pan##A", &oscAPanTemp, 0.0f, 1.0f);
    m_preset->synthOscAPan.store(oscAPanTemp);

    int oscAOctaveTemp = m_preset->synthOscAOctave.load();
    ImGui::SliderInt("Octave##A", &oscAOctaveTemp, 1, 7);
    m_preset->synthOscAOctave.store(oscAOctaveTemp);
}

void PresetWindow::DrawOscillatorB() const {
    ImGui::SeparatorText("Oscillator B");
    
    bool oscBOnTemp = m_preset->synthOscBOn.load();
    ImGui::Checkbox("On##B", &oscBOnTemp);
    m_preset->synthOscBOn.store(oscBOnTemp);
    
    // --- Waveform dropdown ---
    WaveformInfo::Type waveformBTemp = m_preset->synthOscBWaveform.load();

    // ImGui::Combo returns true if the selection changed
    if (ImGui::BeginCombo("Waveform##B", WaveformInfo::Names[static_cast<int>(waveformBTemp)])) {
        for (int n = 0; n < IM_ARRAYSIZE(WaveformInfo::Names); n++) {
            bool isSelected = (static_cast<int>(waveformBTemp) == n);
            if (ImGui::Selectable(WaveformInfo::Names[n], isSelected))
                waveformBTemp = static_cast<WaveformInfo::Type>(n);
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    m_preset->synthOscBWaveform.store(waveformBTemp);
    // ---

    float oscBVolumeTemp = m_preset->synthOscBVolume.load();
    ImGui::SliderFloat("Volume##B", &oscBVolumeTemp, 0.0f, 1.0f);
    m_preset->synthOscBVolume.store(oscBVolumeTemp);

    float oscBPanTemp = m_preset->synthOscBPan.load();
    ImGui::SliderFloat("Pan##B", &oscBPanTemp, 0.0f, 1.0f);
    m_preset->synthOscBPan.store(oscBPanTemp);

    int oscBOctaveTemp = m_preset->synthOscBOctave.load();
    ImGui::SliderInt("Octave##B", &oscBOctaveTemp, 1, 7);
    m_preset->synthOscBOctave.store(oscBOctaveTemp);
}

void PresetWindow::DrawVolumeEnvelope() const {
    ImGui::SeparatorText("Volume envelope");

    // --- Attack ---
    float oscAttackTemp = m_preset->synthOscAttack.load();
    ImGui::SliderFloat("Attack (s)##vol", &oscAttackTemp, 0.0f, 2.0f);
    m_preset->synthOscAttack.store(oscAttackTemp);

    // --- Hold ---
    float oscHoldTemp = m_preset->synthOscHold.load();
    ImGui::SliderFloat("Hold (s)##vol", &oscHoldTemp, 0.0f, 2.0f);
    m_preset->synthOscHold.store(oscHoldTemp);

    // --- Decay ---
    float oscDecTemp = m_preset->synthOscDec.load();
    ImGui::SliderFloat("Decay (s)##vol", &oscDecTemp, 0.0f, 2.0f);
    m_preset->synthOscDec.store(oscDecTemp);

    // --- Sustain ---
    float oscSusTemp = m_preset->synthOscSus.load();
    ImGui::SliderFloat("Sustain (level)##vol", &oscSusTemp, 0.0f, 1.0f);
    m_preset->synthOscSus.store(oscSusTemp);

    // --- Release ---
    float oscRelTemp = m_preset->synthOscRel.load();
    ImGui::SliderFloat("Release (s)##vol", &oscRelTemp, 0.0f, 4.0f);
    m_preset->synthOscRel.store(oscRelTemp);
}

void PresetWindow::DrawLowPassFilter() const {
    ImGui::SeparatorText("Low-pass filter");

    bool lpFilterOnTemp = m_preset->synthLpFilterOn.load();
    ImGui::Checkbox("On##LP", &lpFilterOnTemp);
    m_preset->synthLpFilterOn.store(lpFilterOnTemp);

    float lpFilterMixTemp = m_preset->synthLpFilterMix.load();
    ImGui::SliderFloat("Mix##LP", &lpFilterMixTemp, 0.0f, 1.0f);
    m_preset->synthLpFilterMix.store(lpFilterMixTemp);

    float lpFilterCutoffTemp = m_preset->synthLpFilterCutoff.load();
    ImGui::SliderFloat("Cutoff frequency (Hz)##LP", &lpFilterCutoffTemp,
                    20.0f, 20000.0f, "%.1f Hz", ImGuiSliderFlags_Logarithmic);
    m_preset->synthLpFilterCutoff.store(lpFilterCutoffTemp);

    float lpFilterQTemp = m_preset->synthLpFilterQ.load();
    ImGui::SliderFloat("Peaking/Q##LP", &lpFilterQTemp, 0.1f, 3.0f);
    m_preset->synthLpFilterQ.store(lpFilterQTemp);


    ImGui::SeparatorText("Low-pass filter cutoff envelope");
    
    float oscLpCutoffAttackTemp = m_preset->synthOscLpCutoffAttack.load();
    ImGui::SliderFloat("Attack (s)##cut", &oscLpCutoffAttackTemp, 0.0f, 2.0f);
    m_preset->synthOscLpCutoffAttack.store(oscLpCutoffAttackTemp);

    float oscLpCutoffDecTemp = m_preset->synthOscLpCutoffDec.load();
    ImGui::SliderFloat("Decay (s)##cut", &oscLpCutoffDecTemp, 0.0f, 2.0f);
    m_preset->synthOscLpCutoffDec.store(oscLpCutoffDecTemp);

    float oscLpCutoffAmountTemp = m_preset->synthOscLpCutoffAmount.load();
    ImGui::SliderFloat("Amount (semitones)##cut", &oscLpCutoffAmountTemp, -60.0f, 60.0f);
    m_preset->synthOscLpCutoffAmount.store(oscLpCutoffAmountTemp);
}

void PresetWindow::DrawHighPassFilter() const {
    ImGui::SeparatorText("High-pass filter");

    bool hpFilterOnTemp = m_preset->synthHpFilterOn.load();
    ImGui::Checkbox("On##HP", &hpFilterOnTemp);
    m_preset->synthHpFilterOn.store(hpFilterOnTemp);

    float hpFilterMixTemp = m_preset->synthHpFilterMix.load();
    ImGui::SliderFloat("Mix##HP", &hpFilterMixTemp, 0.0f, 1.0f);
    m_preset->synthHpFilterMix.store(hpFilterMixTemp);

    float hpFilterCutoffTemp = m_preset->synthHpFilterCutoff.load();
    ImGui::SliderFloat("Cutoff frequency (Hz) ##HP", &hpFilterCutoffTemp,
                    20.0f, 20000.0f, "%.1f Hz", ImGuiSliderFlags_Logarithmic);
    m_preset->synthHpFilterCutoff.store(hpFilterCutoffTemp);

    float hpFilterQTemp = m_preset->synthHpFilterQ.load();
    ImGui::SliderFloat("HP Peaking/Q##HP", &hpFilterQTemp, 0.1f, 3.0f);
    m_preset->synthHpFilterQ.store(hpFilterQTemp);
}

void PresetWindow::DrawFeedbackDelay() const {
    ImGui::SeparatorText("Feedback delay");

    bool delayOnTemp = m_preset->synthDelayOn.load();
    ImGui::Checkbox("On##Delay", &delayOnTemp);
    m_preset->synthDelayOn.store(delayOnTemp);

    // --- Delay type dropdown --- 
    FeedbackDelayInfo::Type delayTypeTemp = m_preset->synthDelayType.load();

    if (ImGui::BeginCombo("Delay mode", FeedbackDelayInfo::Names[static_cast<int>(delayTypeTemp)])) {
        for (int n = 0; n < IM_ARRAYSIZE(FeedbackDelayInfo::Names); n++) {
            bool isSelected = (static_cast<int>(delayTypeTemp) == n);
            if (ImGui::Selectable(FeedbackDelayInfo::Names[n], isSelected)) {
                delayTypeTemp = static_cast<FeedbackDelayInfo::Type>(n);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    m_preset->synthDelayType.store(delayTypeTemp);

    float delayMixTemp = m_preset->synthDelayMix.load();
    ImGui::SliderFloat("Mix##Delay", &delayMixTemp, 0.0f, 1.0f);
    m_preset->synthDelayMix.store(delayMixTemp);

    float delayTimeTemp = m_preset->synthDelayTime.load();
    ImGui::SliderFloat("Delay time (s)##Delay", &delayTimeTemp, FeedbackDelayLine::MIN_DELAY_SEC.count(), FeedbackDelayLine::MAX_DELAY_SEC.count());
    m_preset->synthDelayTime.store(delayTimeTemp);

    float delayFeedbackTemp = m_preset->synthDelayFeedback.load();
    ImGui::SliderFloat("Feedback level##Delay", &delayFeedbackTemp, FeedbackDelayLine::MIN_FEEDBACK, FeedbackDelayLine::MAX_FEEDBACK);
    m_preset->synthDelayFeedback.store(delayFeedbackTemp);
}

void PresetWindow::DrawReverb() const {
    ImGui::SeparatorText("Reverb settings");

    bool reverbOnTemp = m_preset->synthReverbOn.load();
    ImGui::Checkbox("On##Reverb", &reverbOnTemp);
    m_preset->synthReverbOn.store(reverbOnTemp);

    float reverbFeedbackTemp = m_preset->synthReverbFeedback.load();
    ImGui::SliderFloat("Feedback level##Reverb", &reverbFeedbackTemp, 0.0f, 0.8f);
    m_preset->synthReverbFeedback.store(reverbFeedbackTemp);

    float reverbDampTemp = m_preset->synthReverbDamp.load();
    ImGui::SliderFloat("Damp level##Reverb", &reverbDampTemp, 0.0f, 1.0f);
    m_preset->synthReverbDamp.store(reverbDampTemp);

    float reverbWetTemp = m_preset->synthReverbWet.load();
    ImGui::SliderFloat("Wet level / Mix##Reverb", &reverbWetTemp, 0.0f, 1.0f);
    m_preset->synthReverbWet.store(reverbWetTemp);
}

void PresetWindow::DrawLFO1() const {
    ImGui::SeparatorText("LFO 1");

    bool LFO1OnTemp = m_preset->synthLFO1On.load();
    ImGui::Checkbox("On##LFO1", &LFO1OnTemp);
    m_preset->synthLFO1On.store(LFO1OnTemp);

    // --- LFO 1 mode drop down --- 
    LFOConfig::Mode lfo1ModeTemp = m_preset->synthLFO1Mode.load();

    if (ImGui::BeginCombo("Mode##LFO1", LFOConfig::ModeNames[static_cast<int>(lfo1ModeTemp)])) {
        for (int n = 0; n < IM_ARRAYSIZE(LFOConfig::ModeNames); n++) {
            bool isSelected = (static_cast<int>(lfo1ModeTemp) == n);
            if (ImGui::Selectable(LFOConfig::ModeNames[n], isSelected)) {
                lfo1ModeTemp = static_cast<LFOConfig::Mode>(n);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    m_preset->synthLFO1Mode.store(lfo1ModeTemp);

    // --- LFO 1 destination drop down --- 
    LFOConfig::Destination lfo1DestinationTemp = m_preset->synthLFO1Destination.load();
    std::vector<std::string> destinationNames = LFOConfig::GetDestinationNames();

    if (ImGui::BeginCombo("Destination##LFO1", destinationNames[static_cast<int>(lfo1DestinationTemp)].c_str())) {
        for (int n = 0; n < std::ssize(LFOConfig::DESTINATION_INFOS); n++) {
            bool isSelected = (static_cast<int>(lfo1DestinationTemp) == n);
            if (ImGui::Selectable(destinationNames[n].c_str(), isSelected)) {
                lfo1DestinationTemp = static_cast<LFOConfig::Destination>(n);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    m_preset->synthLFO1Destination.store(lfo1DestinationTemp);

    LFOConfig::DestinationInfo destInfo = LFOConfig::GetDestinationInfo(lfo1DestinationTemp);
    
    // Amount slider
    float lfo1AmountTemp = m_preset->synthLFO1Amount.load();
    lfo1AmountTemp = std::clamp(lfo1AmountTemp, destInfo.minValue, destInfo.maxValue);
    ImGuiSliderFlags_ sliderFlags = destInfo.isLogarithmic ? ImGuiSliderFlags_Logarithmic : ImGuiSliderFlags_None;
    const char *format = destInfo.format.has_value() ? destInfo.format->c_str() : nullptr;

    ImGui::SliderFloat("Amount##LFO1", &lfo1AmountTemp, destInfo.minValue, destInfo.maxValue, format, sliderFlags);
    m_preset->synthLFO1Amount.store(lfo1AmountTemp);


    switch (lfo1ModeTemp) {
        case LFOConfig::Mode::Envelope: {
            // --- Attack ---
            float attack = m_preset->synthLFO1EnvAttack.load();
            ImGui::SliderFloat("Attack (s)##LFO1", &attack, 0.0f, 2.0f);
            m_preset->synthLFO1EnvAttack.store(attack);

            // --- Hold ---
            float hold = m_preset->synthLFO1EnvHold.load();
            ImGui::SliderFloat("Hold (s)##LFO1", &hold, 0.0f, 2.0f);
            m_preset->synthLFO1EnvHold.store(hold);

            // --- Decay ---
            float dec = m_preset->synthLFO1EnvDec.load();
            ImGui::SliderFloat("Decay (s)##LFO1", &dec, 0.0f, 2.0f);
            m_preset->synthLFO1EnvDec.store(dec);

            // --- Sustain ---
            float sus = m_preset->synthLFO1EnvSus.load();
            ImGui::SliderFloat("Sustain (level)##LFO1", &sus, 0.0f, 1.0f);
            m_preset->synthLFO1EnvSus.store(sus);
            break;
        }
        case LFOConfig::Mode::Periodic: {
            // --- Waveform dropdown ---
            WaveformInfo::Type waveformLFO1Temp = m_preset->synthLFO1Waveform.load();
            if (ImGui::BeginCombo("Waveform##LFO1", WaveformInfo::Names[static_cast<int>(waveformLFO1Temp)])) {
                for (int n = 0; n < IM_ARRAYSIZE(WaveformInfo::Names); n++) {
                    bool isSelected = (static_cast<int>(waveformLFO1Temp) == n);
                    if (ImGui::Selectable(WaveformInfo::Names[n], isSelected)) {
                        waveformLFO1Temp = static_cast<WaveformInfo::Type>(n);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            m_preset->synthLFO1Waveform.store(waveformLFO1Temp);
            // ---

            float freq = m_preset->synthLFO1Frequency.load();
            ImGui::SliderFloat("Frequency (Hz)##LFO1", &freq, 0.1f, 100.0f);
            m_preset->synthLFO1Frequency.store(freq);
            break;
        }
        case LFOConfig::Mode::Random: {
            float freq = m_preset->synthLFO1Frequency.load();
            ImGui::SliderFloat("Frequency (Hz)##LFO1", &freq, 0.1f, 100.0f);
            m_preset->synthLFO1Frequency.store(freq);
            break;
        }
    }
}

void PresetWindow::DrawLFO2() const {
    ImGui::SeparatorText("LFO 2");

    bool LFO2OnTemp = m_preset->synthLFO2On.load();
    ImGui::Checkbox("On##LFO2", &LFO2OnTemp);
    m_preset->synthLFO2On.store(LFO2OnTemp);

    // --- LFO 2 mode drop down --- 
    LFOConfig::Mode lfo2ModeTemp = m_preset->synthLFO2Mode.load();

    if (ImGui::BeginCombo("Mode##LFO2", LFOConfig::ModeNames[static_cast<int>(lfo2ModeTemp)])) {
        for (int n = 0; n < IM_ARRAYSIZE(LFOConfig::ModeNames); n++) {
            bool isSelected = (static_cast<int>(lfo2ModeTemp) == n);
            if (ImGui::Selectable(LFOConfig::ModeNames[n], isSelected)) {
                lfo2ModeTemp = static_cast<LFOConfig::Mode>(n);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    m_preset->synthLFO2Mode.store(lfo2ModeTemp);

    // --- LFO 2 destination drop down --- 
    LFOConfig::Destination lfo2DestinationTemp = m_preset->synthLFO2Destination.load();
    std::vector<std::string> destinationNames = LFOConfig::GetDestinationNames();

    if (ImGui::BeginCombo("Destination##LFO2", destinationNames[static_cast<int>(lfo2DestinationTemp)].c_str())) {
        for (int n = 0; n < std::ssize(LFOConfig::DESTINATION_INFOS); n++) {
            bool isSelected = (static_cast<int>(lfo2DestinationTemp) == n);
            if (ImGui::Selectable(destinationNames[n].c_str(), isSelected)) {
                lfo2DestinationTemp = static_cast<LFOConfig::Destination>(n);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    m_preset->synthLFO2Destination.store(lfo2DestinationTemp);

    LFOConfig::DestinationInfo destInfo = LFOConfig::GetDestinationInfo(lfo2DestinationTemp);
    
    // Amount slider
    float lfo2AmountTemp = m_preset->synthLFO2Amount.load();
    lfo2AmountTemp = std::clamp(lfo2AmountTemp, destInfo.minValue, destInfo.maxValue);
    ImGuiSliderFlags_ sliderFlags = destInfo.isLogarithmic ? ImGuiSliderFlags_Logarithmic : ImGuiSliderFlags_None;
    const char *format = destInfo.format.has_value() ? destInfo.format->c_str() : nullptr;

    ImGui::SliderFloat("Amount##LFO2", &lfo2AmountTemp, destInfo.minValue, destInfo.maxValue, format, sliderFlags);
    m_preset->synthLFO2Amount.store(lfo2AmountTemp);

    switch (lfo2ModeTemp) {
        case LFOConfig::Mode::Envelope: {
            // --- Attack ---
            float attack = m_preset->synthLFO2EnvAttack.load();
            ImGui::SliderFloat("Attack (s)##LFO2", &attack, 0.0f, 2.0f);
            m_preset->synthLFO2EnvAttack.store(attack);

            // --- Hold ---
            float hold = m_preset->synthLFO2EnvHold.load();
            ImGui::SliderFloat("Hold (s)##LFO2", &hold, 0.0f, 2.0f);
            m_preset->synthLFO2EnvHold.store(hold);

            // --- Decay ---
            float dec = m_preset->synthLFO2EnvDec.load();
            ImGui::SliderFloat("Decay (s)##LFO2", &dec, 0.0f, 2.0f);
            m_preset->synthLFO2EnvDec.store(dec);

            // --- Sustain ---
            float sus = m_preset->synthLFO2EnvSus.load();
            ImGui::SliderFloat("Sustain (level)##LFO2", &sus, 0.0f, 1.0f);
            m_preset->synthLFO2EnvSus.store(sus);
            break;
        }
        case LFOConfig::Mode::Periodic: {
            // --- Waveform dropdown ---
            WaveformInfo::Type waveformLFO2Temp = m_preset->synthLFO2Waveform.load();
            if (ImGui::BeginCombo("Waveform##LFO2", WaveformInfo::Names[static_cast<int>(waveformLFO2Temp)])) {
                for (int n = 0; n < IM_ARRAYSIZE(WaveformInfo::Names); n++) {
                    bool isSelected = (static_cast<int>(waveformLFO2Temp) == n);
                    if (ImGui::Selectable(WaveformInfo::Names[n], isSelected)) {
                        waveformLFO2Temp = static_cast<WaveformInfo::Type>(n);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            m_preset->synthLFO2Waveform.store(waveformLFO2Temp);
            // ---

            float freq = m_preset->synthLFO2Frequency.load();
            ImGui::SliderFloat("Frequency (Hz)##LFO2", &freq, 0.1f, 100.0f);
            m_preset->synthLFO2Frequency.store(freq);
            break;
        }
        case LFOConfig::Mode::Random: {
            float freq = m_preset->synthLFO2Frequency.load();
            ImGui::SliderFloat("Frequency (Hz)##LFO2", &freq, 0.1f, 100.0f);
            m_preset->synthLFO2Frequency.store(freq);
            break;
        }
    }
}
