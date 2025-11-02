#include "gui/WaveformWindow.hpp"

#include "gui/Spectrogram.hpp"
#include "gui/LevelsDisplay.hpp"
#include "gui/Keyboard.hpp"

WaveformWindow::WaveformWindow(WaveformInfo::Type waveformType)
    : m_waveformType(waveformType)
{
    UpdateTexture();
}

void WaveformWindow::Render(WaveformInfo::Type waveformType) {
    ImVec2 windowSize = ConfigureWindow();
    ImVec2 imageSize = ImVec2(windowSize.x - IMAGE_PADDING_RIGHT, windowSize.y - IMAGE_PADDING_BOTTOM);

    if (waveformType != m_waveformType) {
        m_waveformType = waveformType;
        UpdateTexture();
    }

    if (m_waveformTex) {
        ImGui::Image((ImTextureID)(intptr_t)m_waveformTex, imageSize);
    }
    
    ImGui::End();
}

ImVec2 WaveformWindow::ConfigureWindow() const {
    // Get viewport (the main window area)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Force window to bottom, full width
    const float x = viewport->Pos.x + viewport->Size.x - Spectrogram::WINDOW_WIDTH - LevelsDisplay::WINDOW_WIDTH;
    const float y = viewport->Pos.y + Spectrogram::WINDOW_HEIGHT;
    const float w = (viewport->Size.x - x) / 2.0f; // Fit two waveform windows in the remaining space
    const float h = (viewport->Size.y - Spectrogram::WINDOW_HEIGHT - Keyboard::WINDOW_HEIGHT) / 2.0f;
    ImVec2 windowSize(w, h);
    ImGui::SetNextWindowPos(
        ImVec2(x, y),
        ImGuiCond_Always
    );
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin("Oscillator A waveform", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::SeparatorText("Oscillator A waveform");

    return windowSize;
}

void WaveformWindow::InitTexture() {
    glGenTextures(1, &m_waveformTex);
    glBindTexture(GL_TEXTURE_2D, m_waveformTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 TEXTURE_WIDTH, TEXTURE_HEIGHT, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void WaveformWindow::UpdateTexture() {
    // Prepare pixels
    std::unique_ptr<Waveform> wf = Waveform::ConstructWaveform(m_waveformType);
    std::vector<unsigned char> pixels(TEXTURE_HEIGHT * TEXTURE_WIDTH * 3, 0);
    int prevRow = 0;
    for (int col = 0; col < TEXTURE_WIDTH; col++) {
        float waveformValue = wf->GetSampleAt(static_cast<float>(col) / (TEXTURE_WIDTH - 1));
        waveformValue = waveformValue / 2.0f + 0.5f;
        int row = static_cast<int>(waveformValue * TEXTURE_HEIGHT);
        row = std::clamp(row, 0, TEXTURE_HEIGHT - 1);

        if (col > 0) {
            // Draw line from (col-1, prevRow) to (col, row)
            int x0 = col - 1, y0 = prevRow;
            int x1 = col, y1 = row;
            int dx = abs(x1 - x0), dy = -abs(y1 - y0);
            int sx = x0 < x1 ? 1 : -1;
            int sy = y0 < y1 ? 1 : -1;
            int err = dx + dy;

            while (true) {
                int pixelIdx = ((TEXTURE_HEIGHT - y0 - 1) * TEXTURE_WIDTH + x0) * 3;
                pixels[pixelIdx + 0] = HIGHLIGHT_COLOR[0];
                pixels[pixelIdx + 1] = HIGHLIGHT_COLOR[1];
                pixels[pixelIdx + 2] = HIGHLIGHT_COLOR[2];
                if (x0 == x1 && y0 == y1) {
                    break;
                }
                int e2 = 2 * err;
                if (e2 >= dy) {
                    err += dy; x0 += sx;
                }
                if (e2 <= dx) {
                    err += dx; y0 += sy;
                }
            }
        }

        prevRow = row;
    }

    // Upload to OpenGL texture
    glBindTexture(GL_TEXTURE_2D, m_waveformTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT,
                    GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
}