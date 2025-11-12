// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "GUIManager.hpp"
#include "ColorThemeApplier.hpp"
#include <utility>
#include <iostream>

// RAII class for managing the GLFW window
GUIManager::GUIManager(std::shared_ptr<Audio::Preset::AudioPreset> preset, std::shared_ptr<FFTComputer> fftComputer)
    : m_preset(preset)
    , m_fftComputer(fftComputer)
    , m_keyboard(SCREEN_WIDTH)
    , m_presetWindow(preset)
    , m_oscAWaveformWindow(preset->synthOscAWaveform.load(), "Oscillator A waveform")
    , m_oscBWaveformWindow(preset->synthOscBWaveform.load(), "Oscillator B waveform")
{
    m_window = InitAux();
    if (m_window == nullptr) {
        std::cerr << "Couldn't initialize window\n";
        exit(1);
    }

    // Now that the GL context and window has been initialized
    m_spectrogram.InitTexture();
    m_levelsDisplay.InitTexture();
    m_oscAWaveformWindow.InitTexture();
    m_oscBWaveformWindow.InitTexture();

    // Find a default midi input device
    m_midiInput.OpenDefaultPort();
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

        // Handle key input BEFORE starting new ImGui frame. Will pass this along to the keyboard class
        std::set<Audio::Core::Note> allPressedNotes = GetAllPressedNotes();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ColorThemeApplier applier; // Applies the color theme and automatically pops at the end of this block.

            m_globalSettingsWindow.Render(*m_preset);
            m_presetLoaderWindow.Render(*m_preset);

            float framerate = m_io->Framerate;
            m_presetWindow.Render(framerate); // Pass framerate to display it at the bottom

            std::shared_ptr<std::vector<float>> column = m_fftComputer->GetLastFFTResult();
            if (column != nullptr) {
                m_spectrogram.PushColumn(*column.get());
                m_spectrumWindow.PushMagnitudes(*column.get());
                m_spectrogram.Render();
                m_spectrumWindow.Render();
            }

            std::shared_ptr<Audio::Engine::AudioFrame> levels = m_fftComputer->GetLastAudioLevels();
            if (levels != nullptr) {
                m_levelsDisplay.UpdateLevels(*levels.get());
                m_levelsDisplay.Render();
            }

            bool isOscAOn = m_preset->synthOscAOn.load();
            bool isOscBOn = m_preset->synthOscBOn.load();
            m_oscAWaveformWindow.Render(m_preset->synthOscAWaveform.load(), true, isOscAOn);
            m_oscBWaveformWindow.Render(m_preset->synthOscBWaveform.load(), false, isOscBOn);

            // Provide keyboard with the Qwerty input since it cannot access it itself.
            allPressedNotes = m_keyboard.Render(allPressedNotes);

            // Store keyboard state (all pressed notes returned) via the shared preset
            for (Audio::Core::Note note = KeyboardWindow::FIRST_NOTE; note <= KeyboardWindow::LAST_NOTE; ++note) {
                int noteIdx = note - KeyboardWindow::FIRST_NOTE;
                bool isPressed = allPressedNotes.find(note) != allPressedNotes.end();
                m_preset->noteStates[noteIdx].store(isPressed);
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

void GUIManager::glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
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
    GLFWwindow* window = glfwCreateWindow((int)(SCREEN_WIDTH), (int)(SCREEN_HEIGHT), "Chirp", nullptr, nullptr);
    if (window == nullptr) {
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO();
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Set background color
    glClearColor(CLEAR_COLOR.x * CLEAR_COLOR.w, CLEAR_COLOR.y * CLEAR_COLOR.w, CLEAR_COLOR.z * CLEAR_COLOR.w, CLEAR_COLOR.w);

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

std::set<Audio::Core::Note> GUIManager::GetQwertyNotesPressed() const {
    using Note = Audio::Core::Note;
    using Key = Audio::Core::Key;

    // Map keys onto notes
    static const std::pair<int, Note> QWERTY_NOTE_MAP[] = {
        // Lower octave
        { GLFW_KEY_Z, Note(Key::C, 4) },
        { GLFW_KEY_S, Note(Key::Cs, 4) },
        { GLFW_KEY_X, Note(Key::D, 4) },
        { GLFW_KEY_D, Note(Key::Ds, 4) },
        { GLFW_KEY_C, Note(Key::E, 4) },
        { GLFW_KEY_V, Note(Key::F, 4) },
        { GLFW_KEY_G, Note(Key::Fs, 4) },
        { GLFW_KEY_B, Note(Key::G, 4) },
        { GLFW_KEY_H, Note(Key::Gs, 4) },
        { GLFW_KEY_N, Note(Key::A, 4) },
        { GLFW_KEY_J, Note(Key::As, 4) },
        { GLFW_KEY_M, Note(Key::B, 4) },

        // Higher octave
        { GLFW_KEY_W, Note(Key::C, 5) },
        { GLFW_KEY_3, Note(Key::Cs, 5) },
        { GLFW_KEY_E, Note(Key::D, 5) },
        { GLFW_KEY_4, Note(Key::Ds, 5) },
        { GLFW_KEY_R, Note(Key::E, 5) },
        { GLFW_KEY_T, Note(Key::F, 5) },
        { GLFW_KEY_6, Note(Key::Fs, 5) },
        { GLFW_KEY_Y, Note(Key::G, 5) },
        { GLFW_KEY_7, Note(Key::Gs, 5) },
        { GLFW_KEY_U, Note(Key::A, 5) },
        { GLFW_KEY_8, Note(Key::As, 5) },
        { GLFW_KEY_I, Note(Key::B, 5) },
    };

    // Check which keys are pressed
    std::set<Note> pressedKeys;
    for (auto const& [glfwKey, note] : QWERTY_NOTE_MAP) {
        if (glfwGetKey(m_window, glfwKey) == GLFW_PRESS) {
            pressedKeys.insert(note);
        }
    }

    return pressedKeys;
}

std::set<Audio::Core::Note> GUIManager::GetAllPressedNotes() {
    std::set<Audio::Core::Note> allPressedNotes = GetQwertyNotesPressed();

    // Get pressed notes from MIDI input
    auto midiNotes = m_midiInput.GetPressedNotes();

    // Merge them
    allPressedNotes.insert(midiNotes.begin(), midiNotes.end());
    return allPressedNotes;
}