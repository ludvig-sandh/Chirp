#include "gui/WaveformWindow.hpp"

#include "gui/Spectrogram.hpp"
#include "gui/LevelsDisplay.hpp"
#include "gui/Keyboard.hpp"
#include "gui/GUIConstants.hpp"

WaveformWindow::WaveformWindow(WaveformInfo::Type waveformType, const std::string& windowName)
    : m_waveformType(waveformType)
    , m_windowName(windowName)
{
    UpdateTexture();
}

void WaveformWindow::Render(WaveformInfo::Type waveformType, bool comesFirst, bool isOn) {
    ImVec2 windowSize = ConfigureWindow(comesFirst);
    ImVec2 imageSize = ImVec2(windowSize.x - IMAGE_PADDING_RIGHT, windowSize.y - IMAGE_PADDING_BOTTOM);

    if (waveformType != m_waveformType) {
        m_waveformType = waveformType;
        UpdateTexture();
    }

    if (m_waveformTex) {
        float opacity = isOn ? ON_OPACITY : OFF_OPACITY;
        ImGui::Image(
            (ImTextureID)(intptr_t)m_waveformTex,
            imageSize,
            ImVec2(0,0), // uv0
            ImVec2(1,1), // uv1
            ImVec4(1, 1, 1, opacity), // Tint color
            ImVec4(0, 0, 0, 0) // Border
        );
    }
    
    ImGui::End();
}

ImVec2 WaveformWindow::ConfigureWindow(bool comesFirst) const {
    // Get viewport (the main window area)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    const float xFirstSlot = viewport->Pos.x + viewport->Size.x - Spectrogram::WINDOW_WIDTH - LevelsDisplay::WINDOW_WIDTH;
    const float y = viewport->Pos.y + Spectrogram::WINDOW_HEIGHT;
    const float w = (viewport->Size.x - xFirstSlot) / 2.0f; // Fit two waveform windows in this space
    const float h = (viewport->Size.y - Spectrogram::WINDOW_HEIGHT - Keyboard::WINDOW_HEIGHT) / 2.0f;
    const float xSecondSlot = xFirstSlot + w;

    ImVec2 windowSize(w, h);
    ImGui::SetNextWindowPos(
        ImVec2(comesFirst ? xFirstSlot : xSecondSlot, y),
        ImGuiCond_Always
    );
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    // Create a non-movable, non-collapsible, non-resizable, no-title-bar panel
    ImGui::Begin(m_windowName.c_str(), nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::SeparatorText(m_windowName.c_str());

    return windowSize;
}

void WaveformWindow::InitTexture() {
    glGenTextures(1, &m_waveformTex);
    glBindTexture(GL_TEXTURE_2D, m_waveformTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 TEXTURE_WIDTH, TEXTURE_HEIGHT, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void WaveformWindow::UpdateTexture() {
    if (m_waveformTex == 0) {
        // If texture hasn't been initialized yet, we shouldn't call glBindTexture on it.
        return;
    }
    
    // Prepare pixels
    std::unique_ptr<Waveform> wf = Waveform::ConstructWaveform(m_waveformType);
    std::vector<unsigned char> pixels(TEXTURE_HEIGHT * TEXTURE_WIDTH * 4, 0);

    // Convenient helper function to paint a pixel with a thick brush (a circle). With radius=0 paints only 1 pixel
    auto paintFn = [&](int row, int col, int radius, const std::array<unsigned char, 4>& color){
        for (int dc = -radius; dc <= radius; dc++) {
            for (int dr = -radius; dr <= radius; dr++) {
                // Check bounds
                int newCol = col + dc;
                int newRow = row + dr;
                if (newCol < 0 || newCol >= TEXTURE_WIDTH) {
                    continue;
                }
                if (newRow < 0 || newRow >= TEXTURE_HEIGHT) {
                    continue;
                }

                // Paint a circle => pixels must be within radius
                float dist = std::hypotf(static_cast<float>(dc), static_cast<float>(dr));
                if (dist > radius) {
                    continue;
                }

                int pixelIdx = ((TEXTURE_HEIGHT - newRow - 1) * TEXTURE_WIDTH + newCol) * 4;
                pixels[pixelIdx + 0] = color[0];
                pixels[pixelIdx + 1] = color[1];
                pixels[pixelIdx + 2] = color[2];
                pixels[pixelIdx + 3] = color[3];
            }
        }
    };

    int prevRow = 0;
    for (int col = 0; col < TEXTURE_WIDTH; col++) {
        float waveformValue = wf->GetSampleAt(static_cast<float>(col) / (TEXTURE_WIDTH - 1));
        waveformValue *= 0.9f; // Looks nicer if the waveform doesn't touch the top/bottom image edges
        waveformValue = waveformValue / 2.0f + 0.5f; // Go from range [-1, 1] to [0, 1]
        int row = static_cast<int>(waveformValue * TEXTURE_HEIGHT);
        row = std::clamp(row, 0, TEXTURE_HEIGHT - 1);

        // Paint the "shadow" of the waveform
        const int midRow = TEXTURE_HEIGHT / 2;
        for (int shadowRow = std::min(row, midRow); shadowRow < std::max(row, midRow); shadowRow++) {
            paintFn(shadowRow, col, 0, GUIConstants::Colors::MIDTONE);
        }

        // Linear interpolation between last point and this one: (col-1, prevRow) to (col, row)
        if (col > 0) {
            int x0 = col - 1, y0 = prevRow;
            int x1 = col, y1 = row;
            int dx = abs(x1 - x0), dy = -abs(y1 - y0);
            int sx = x0 < x1 ? 1 : -1;
            int sy = y0 < y1 ? 1 : -1;
            int err = dx + dy;

            while (true) {
                paintFn(y0, x0, 1, GUIConstants::Colors::HIGHLIGHT);
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
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
}
