// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <GLFW/glfw3.h>
#include <vector>

namespace GUI::Window::Helper {

class SpectrogramRenderer {
public:
    ~SpectrogramRenderer();

    bool Init();
    void Resize(int width, int height);
    void PushColumn(const std::vector<float>& magnitudes, int column);
    void Shutdown();

    GLuint TextureId() const;

private:
    bool CreateProgram();
    void RenderTexture(int currentColumn);

    GLuint m_spectrogramTex = 0;
    GLuint m_magnitudeTex = 0;
    GLuint m_framebuffer = 0;
    GLuint m_shaderProgram = 0;
    GLuint m_quadVao = 0;
    GLuint m_quadVbo = 0;

    GLint m_magnitudesUniform = -1;
    GLint m_currentColumnUniform = -1;
    GLint m_sizeUniform = -1;
    GLint m_midtoneUniform = -1;
    GLint m_highlightUniform = -1;

    int m_width = 0;
    int m_height = 0;
    bool m_initialized = false;
};

} // namespace GUI::Window::Helper
