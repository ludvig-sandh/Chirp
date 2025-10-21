// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "GUIManager.hpp"
#include "AudioBackend.hpp"
#include "Waveform.hpp"
#include "AppMode.hpp"
#include <utility>
#include <iostream>

// RAII class for managing the GLFW window
GUIManager::GUIManager(std::shared_ptr<AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer)
    : m_preset(preset)
    , m_fftComputer(fftComputer) {
    m_window = InitAux();
    if (m_window == nullptr) {
        std::cerr << "Couldn't initialize window\n";
        exit(0);
    }

    // Now that the GL context and window has been initialized
    m_spectrogram.InitTexture();
    m_levelsDisplay.InitTexture();
}

GUIManager::~GUIManager() {
    DeinitAux();
}

void GUIManager::RunMainLoop() {
    while (!glfwWindowShouldClose(m_window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Handle key input BEFORE starting new ImGui frame
        HandleKeyboardInput();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("Preset control"); // Create a window

            AppMode currentModeTemp = m_preset->appMode.load();
            if (ImGui::BeginTabBar("ModeTabs")) {
                if (ImGui::BeginTabItem("Chirp mode")) {
                    currentModeTemp = AppMode::Chirp;
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Synth mode")) {
                    currentModeTemp = AppMode::Synth;
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            m_preset->appMode.store(currentModeTemp);

            switch (currentModeTemp) {
                case AppMode::Chirp:
                    DrawChirpUI();
                    break;
                case AppMode::Synth:
                    DrawSynthUI();
                    break;
            }
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
    }
}

void GUIManager::DrawChirpUI() {
    ImGui::Text("Global settings");

    float volumeTemp = m_preset->chirpMasterVolume.load();
    ImGui::SliderFloat("Master volume", &volumeTemp, 0.0f, 1.0f);
    m_preset->chirpMasterVolume.store(volumeTemp);

    ImGui::Separator();
    ImGui::Text("Chirp settings");

    bool chirpOnTemp = m_preset->chirpOn.load();
    ImGui::Checkbox("Chirp sounds", &chirpOnTemp);
    m_preset->chirpOn.store(chirpOnTemp);

    float chirpVolumeTemp = m_preset->chirpVolume.load();
    ImGui::SliderFloat("Chirp volume", &chirpVolumeTemp, 0.0f, 1.0f);
    m_preset->chirpVolume.store(chirpVolumeTemp);

    float chirpPanTemp = m_preset->chirpPan.load();
    ImGui::SliderFloat("Chirp pan", &chirpPanTemp, 0.0f, 1.0f);
    m_preset->chirpPan.store(chirpPanTemp);

    ImGui::Separator();
    ImGui::Text("Noise settings");

    bool noiseOnTemp = m_preset->chirpNoiseOn.load();
    ImGui::Checkbox("White noise", &noiseOnTemp);
    m_preset->chirpNoiseOn.store(noiseOnTemp);

    float noiseVolumeTemp = m_preset->chirpNoiseVolume.load();
    ImGui::SliderFloat("White noise level", &noiseVolumeTemp, 0.0f, 1.0f);
    m_preset->chirpNoiseVolume.store(noiseVolumeTemp);

    ImGui::Separator();
    ImGui::Text("LP filter settings");

    bool lpFilterOnTemp = m_preset->chirpLpFilterOn.load();
    ImGui::Checkbox("Low-pass filter", &lpFilterOnTemp);
    m_preset->chirpLpFilterOn.store(lpFilterOnTemp);

    float lpFilterMixTemp = m_preset->chirpLpFilterMix.load();
    ImGui::SliderFloat("LP Filter mix", &lpFilterMixTemp, 0.0f, 1.0f);
    m_preset->chirpLpFilterMix.store(lpFilterMixTemp);

    float lpFilterCutoffTemp = m_preset->chirpLpFilterCutoff.load();
    ImGui::SliderFloat("LP Cutoff frequency (Hz)", &lpFilterCutoffTemp, 0.0f, 20000.0f);
    m_preset->chirpLpFilterCutoff.store(lpFilterCutoffTemp);

    float lpFilterQTemp = m_preset->chirpLpFilterQ.load();
    ImGui::SliderFloat("LP Peaking/Q", &lpFilterQTemp, 0.0f, 3.0f);
    m_preset->chirpLpFilterQ.store(lpFilterQTemp);

    ImGui::Separator();
    ImGui::Text("HP filter settings");

    bool hpFilterOnTemp = m_preset->chirpHpFilterOn.load();
    ImGui::Checkbox("High-pass filter", &hpFilterOnTemp);
    m_preset->chirpHpFilterOn.store(hpFilterOnTemp);

    float hpFilterMixTemp = m_preset->chirpHpFilterMix.load();
    ImGui::SliderFloat("HP Filter mix", &hpFilterMixTemp, 0.0f, 1.0f);
    m_preset->chirpHpFilterMix.store(hpFilterMixTemp);

    float hpFilterCutoffTemp = m_preset->chirpHpFilterCutoff.load();
    ImGui::SliderFloat("HP Cutoff frequency (Hz)", &hpFilterCutoffTemp, 0.0f, 20000.0f);
    m_preset->chirpHpFilterCutoff.store(hpFilterCutoffTemp);

    float hpFilterQTemp = m_preset->chirpHpFilterQ.load();
    ImGui::SliderFloat("HP Peaking/Q", &hpFilterQTemp, 0.0f, 3.0f);
    m_preset->chirpHpFilterQ.store(hpFilterQTemp);

    ImGui::Separator();
    ImGui::Text("Reverb settings");

    bool reverbOnTemp = m_preset->chirpReverbOn.load();
    ImGui::Checkbox("Reverb", &reverbOnTemp);
    m_preset->chirpReverbOn.store(reverbOnTemp);

    float reverbFeedbackTemp = m_preset->chirpReverbFeedback.load();
    ImGui::SliderFloat("Reverb feedback", &reverbFeedbackTemp, 0.0f, 0.8f);
    m_preset->chirpReverbFeedback.store(reverbFeedbackTemp);

    float reverbDampTemp = m_preset->chirpReverbDamp.load();
    ImGui::SliderFloat("Reverb damp", &reverbDampTemp, 0.0f, 1.0f);
    m_preset->chirpReverbDamp.store(reverbDampTemp);

    float reverbWetTemp = m_preset->chirpReverbWet.load();
    ImGui::SliderFloat("Reverb wet", &reverbWetTemp, 0.0f, 1.0f);
    m_preset->chirpReverbWet.store(reverbWetTemp);

    
    std::shared_ptr<std::vector<float>> column = m_fftComputer->GetLastFFTResult();
    if (column != nullptr) {
        m_spectrogram.PushColumn(*column.get());
        m_spectrogram.Show();
    }

    std::shared_ptr<AudioBufferFrame> levels = m_fftComputer->GetLastAudioLevels();
    if (levels != nullptr) {
        m_levelsDisplay.UpdateLevels(*levels.get());
        m_levelsDisplay.Show();
    }

    float framerate = m_io->Framerate;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    ImGui::End();
}

void GUIManager::DrawSynthUI() {
    ImGui::Text("Global settings");

    float volumeTemp = m_preset->synthMasterVolume.load();
    ImGui::SliderFloat("Master volume", &volumeTemp, 0.0f, 1.0f);
    m_preset->synthMasterVolume.store(volumeTemp);

    ImGui::Separator();
    ImGui::Text("Oscillator A");
    
    bool oscAOnTemp = m_preset->synthOscAOn.load();
    ImGui::Checkbox("On", &oscAOnTemp);
    m_preset->synthOscAOn.store(oscAOnTemp);
    
    // --- Waveform dropdown ---
    WaveformInfo::Type waveformTemp = m_preset->synthOscAWaveform.load();

    // ImGui::Combo returns true if the selection changed
    if (ImGui::BeginCombo("Waveform", WaveformInfo::Names[static_cast<int>(waveformTemp)])) {
        for (int n = 0; n < IM_ARRAYSIZE(WaveformInfo::Names); n++) {
            bool isSelected = (static_cast<int>(waveformTemp) == n);
            if (ImGui::Selectable(WaveformInfo::Names[n], isSelected))
                waveformTemp = static_cast<WaveformInfo::Type>(n);
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    m_preset->synthOscAWaveform.store(waveformTemp);
    // ---

    float oscAVolumeTemp = m_preset->synthOscAVolume.load();
    ImGui::SliderFloat("Volume", &oscAVolumeTemp, 0.0f, 1.0f);
    m_preset->synthOscAVolume.store(oscAVolumeTemp);

    float oscAPanTemp = m_preset->synthOscAPan.load();
    ImGui::SliderFloat("Pan", &oscAPanTemp, 0.0f, 1.0f);
    m_preset->synthOscAPan.store(oscAPanTemp);

    int oscAOctaveTemp = m_preset->synthOscAOctave.load();
    ImGui::SliderInt("Octave", &oscAOctaveTemp, 1, 7);
    m_preset->synthOscAOctave.store(oscAOctaveTemp);


    ImGui::Separator();
    ImGui::Text("Oscillator A envelope");

    // --- Attack ---
    float oscAAttackTemp = m_preset->synthOscAAttack.load();
    ImGui::SliderFloat("Attack (s)", &oscAAttackTemp, 0.0f, 2.0f);
    m_preset->synthOscAAttack.store(oscAAttackTemp);

    // --- Hold ---
    float oscAHoldTemp = m_preset->synthOscAHold.load();
    ImGui::SliderFloat("Hold (s)", &oscAHoldTemp, 0.0f, 2.0f);
    m_preset->synthOscAHold.store(oscAHoldTemp);

    // --- Decay ---
    float oscADecTemp = m_preset->synthOscADec.load();
    ImGui::SliderFloat("Decay (s)", &oscADecTemp, 0.0f, 2.0f);
    m_preset->synthOscADec.store(oscADecTemp);

    // --- Sustain ---
    float oscASusTemp = m_preset->synthOscASus.load();
    ImGui::SliderFloat("Sustain (level)", &oscASusTemp, 0.0f, 1.0f);
    m_preset->synthOscASus.store(oscASusTemp);


    ImGui::Separator();
    ImGui::Text("Oscillator A LP filter settings");

    bool lpFilterOnTemp = m_preset->synthLpFilterOn.load();
    ImGui::Checkbox("Low-pass filter", &lpFilterOnTemp);
    m_preset->synthLpFilterOn.store(lpFilterOnTemp);

    float lpFilterMixTemp = m_preset->synthLpFilterMix.load();
    ImGui::SliderFloat("Mix", &lpFilterMixTemp, 0.0f, 1.0f);
    m_preset->synthLpFilterMix.store(lpFilterMixTemp);

    float lpFilterCutoffTemp = m_preset->synthLpFilterCutoff.load();
    ImGui::SliderFloat("Cutoff frequency (Hz)", &lpFilterCutoffTemp, 0.0f, 20000.0f);
    m_preset->synthLpFilterCutoff.store(lpFilterCutoffTemp);

    float lpFilterQTemp = m_preset->synthLpFilterQ.load();
    ImGui::SliderFloat("Peaking/Q", &lpFilterQTemp, 0.0f, 3.0f);
    m_preset->synthLpFilterQ.store(lpFilterQTemp);

    
    float oscALpCutoffAttackTemp = m_preset->synthOscALpCutoffAttack.load();
    ImGui::SliderFloat("Cutoff envelope attack (s)", &oscALpCutoffAttackTemp, 0.0f, 2.0f);
    m_preset->synthOscALpCutoffAttack.store(oscALpCutoffAttackTemp);

    float oscALpCutoffDecTemp = m_preset->synthOscALpCutoffDec.load();
    ImGui::SliderFloat("Cutoff envelope decay (s)", &oscALpCutoffDecTemp, 0.0f, 2.0f);
    m_preset->synthOscALpCutoffDec.store(oscALpCutoffDecTemp);

    float oscALpCutoffAmountTemp = m_preset->synthOscALpCutoffAmount.load();
    ImGui::SliderFloat("Cutoff envelope amount (semitones)", &oscALpCutoffAmountTemp, -60.0f, 60.0f);
    m_preset->synthOscALpCutoffAmount.store(oscALpCutoffAmountTemp);


    ImGui::Separator();
    ImGui::Text("HP filter settings");

    bool hpFilterOnTemp = m_preset->synthHpFilterOn.load();
    ImGui::Checkbox("High-pass filter", &hpFilterOnTemp);
    m_preset->synthHpFilterOn.store(hpFilterOnTemp);

    float hpFilterMixTemp = m_preset->synthHpFilterMix.load();
    ImGui::SliderFloat("HP Filter mix", &hpFilterMixTemp, 0.0f, 1.0f);
    m_preset->synthHpFilterMix.store(hpFilterMixTemp);

    float hpFilterCutoffTemp = m_preset->synthHpFilterCutoff.load();
    ImGui::SliderFloat("HP Cutoff frequency (Hz)", &hpFilterCutoffTemp, 0.0f, 20000.0f);
    m_preset->synthHpFilterCutoff.store(hpFilterCutoffTemp);

    float hpFilterQTemp = m_preset->synthHpFilterQ.load();
    ImGui::SliderFloat("HP Peaking/Q", &hpFilterQTemp, 0.0f, 3.0f);
    m_preset->synthHpFilterQ.store(hpFilterQTemp);

    ImGui::Separator();
    ImGui::Text("Reverb settings");

    bool reverbOnTemp = m_preset->synthReverbOn.load();
    ImGui::Checkbox("Reverb", &reverbOnTemp);
    m_preset->synthReverbOn.store(reverbOnTemp);

    float reverbFeedbackTemp = m_preset->synthReverbFeedback.load();
    ImGui::SliderFloat("Reverb feedback", &reverbFeedbackTemp, 0.0f, 0.8f);
    m_preset->synthReverbFeedback.store(reverbFeedbackTemp);

    float reverbDampTemp = m_preset->synthReverbDamp.load();
    ImGui::SliderFloat("Reverb damp", &reverbDampTemp, 0.0f, 1.0f);
    m_preset->synthReverbDamp.store(reverbDampTemp);

    float reverbWetTemp = m_preset->synthReverbWet.load();
    ImGui::SliderFloat("Reverb wet", &reverbWetTemp, 0.0f, 1.0f);
    m_preset->synthReverbWet.store(reverbWetTemp);

    
    std::shared_ptr<std::vector<float>> column = m_fftComputer->GetLastFFTResult();
    if (column != nullptr) {
        m_spectrogram.PushColumn(*column.get());
        m_spectrogram.Show();
    }

    std::shared_ptr<AudioBufferFrame> levels = m_fftComputer->GetLastAudioLevels();
    if (levels != nullptr) {
        m_levelsDisplay.UpdateLevels(*levels.get());
        m_levelsDisplay.Show();
    }

    float framerate = m_io->Framerate;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    ImGui::End();
}

void GUIManager::glfw_error_callback(int error, const char* description)
{
    std::cerr << std::format("GLFW Error {}: {}\n", error, description);
}

GLFWwindow *GUIManager::InitAux() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return nullptr;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "Chirp Realtime Audio Synthesis", nullptr, nullptr);
    if (window == nullptr)
        return nullptr;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO();
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Set background color
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details. If you like the default font but want it to scale better, consider using the 'ProggyVector' from the same author!
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    return window;
}

void GUIManager::DeinitAux() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void GUIManager::HandleKeyboardInput() {
    m_preset->noteA5.store(glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS);
    m_preset->noteAs5.store(glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS);
    m_preset->noteB5.store(glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS);
    m_preset->noteC5.store(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS);
    m_preset->noteCs5.store(glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS);
    m_preset->noteD5.store(glfwGetKey(m_window, GLFW_KEY_V) == GLFW_PRESS);
    m_preset->noteDs5.store(glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS);
    m_preset->noteE5.store(glfwGetKey(m_window, GLFW_KEY_B) == GLFW_PRESS);
    m_preset->noteF5.store(glfwGetKey(m_window, GLFW_KEY_H) == GLFW_PRESS);
    m_preset->noteFs5.store(glfwGetKey(m_window, GLFW_KEY_N) == GLFW_PRESS);
    m_preset->noteG5.store(glfwGetKey(m_window, GLFW_KEY_J) == GLFW_PRESS);
    m_preset->noteGs5.store(glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS);
}