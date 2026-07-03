// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "gui/windows/helper/SpectrogramRenderer.hpp"
#include "gui/GUIConstants.hpp"

#include <cstddef>
#include <iostream>

#ifndef GL_R32F
#define GL_R32F 0x822E
#endif
#ifndef GL_RED
#define GL_RED 0x1903
#endif
#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif
#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif
#ifndef GL_FRAMEBUFFER_BINDING
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif
#ifndef GL_CURRENT_PROGRAM
#define GL_CURRENT_PROGRAM 0x8B8D
#endif
#ifndef GL_ACTIVE_TEXTURE
#define GL_ACTIVE_TEXTURE 0x84E0
#endif
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_ARRAY_BUFFER_BINDING
#define GL_ARRAY_BUFFER_BINDING 0x8894
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif
#ifndef GL_VERTEX_ARRAY_BINDING
#define GL_VERTEX_ARRAY_BINDING 0x85B5
#endif
#ifndef GL_TRIANGLE_STRIP
#define GL_TRIANGLE_STRIP 0x0005
#endif

namespace {

struct OpenGLFunctions {
    using GlCreateShaderFn = GLuint(APIENTRY *)(GLenum type);
    using GlShaderSourceFn = void(APIENTRY *)(GLuint shader, GLsizei count, const char* const* string, const GLint* length);
    using GlCompileShaderFn = void(APIENTRY *)(GLuint shader);
    using GlGetShaderivFn = void(APIENTRY *)(GLuint shader, GLenum pname, GLint* params);
    using GlGetShaderInfoLogFn = void(APIENTRY *)(GLuint shader, GLsizei bufSize, GLsizei* length, char* infoLog);
    using GlCreateProgramFn = GLuint(APIENTRY *)();
    using GlAttachShaderFn = void(APIENTRY *)(GLuint program, GLuint shader);
    using GlLinkProgramFn = void(APIENTRY *)(GLuint program);
    using GlGetProgramivFn = void(APIENTRY *)(GLuint program, GLenum pname, GLint* params);
    using GlGetProgramInfoLogFn = void(APIENTRY *)(GLuint program, GLsizei bufSize, GLsizei* length, char* infoLog);
    using GlDeleteShaderFn = void(APIENTRY *)(GLuint shader);
    using GlDeleteProgramFn = void(APIENTRY *)(GLuint program);
    using GlUseProgramFn = void(APIENTRY *)(GLuint program);
    using GlGetUniformLocationFn = GLint(APIENTRY *)(GLuint program, const char* name);
    using GlGetAttribLocationFn = GLint(APIENTRY *)(GLuint program, const char* name);
    using GlUniform1iFn = void(APIENTRY *)(GLint location, GLint v0);
    using GlUniform2iFn = void(APIENTRY *)(GLint location, GLint v0, GLint v1);
    using GlUniform4fFn = void(APIENTRY *)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    using GlActiveTextureFn = void(APIENTRY *)(GLenum texture);
    using GlGenFramebuffersFn = void(APIENTRY *)(GLsizei n, GLuint* framebuffers);
    using GlBindFramebufferFn = void(APIENTRY *)(GLenum target, GLuint framebuffer);
    using GlFramebufferTexture2DFn = void(APIENTRY *)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    using GlCheckFramebufferStatusFn = GLenum(APIENTRY *)(GLenum target);
    using GlDeleteFramebuffersFn = void(APIENTRY *)(GLsizei n, const GLuint* framebuffers);
    using GlGenVertexArraysFn = void(APIENTRY *)(GLsizei n, GLuint* arrays);
    using GlBindVertexArrayFn = void(APIENTRY *)(GLuint array);
    using GlDeleteVertexArraysFn = void(APIENTRY *)(GLsizei n, const GLuint* arrays);
    using GlGenBuffersFn = void(APIENTRY *)(GLsizei n, GLuint* buffers);
    using GlBindBufferFn = void(APIENTRY *)(GLenum target, GLuint buffer);
    using GlBufferDataFn = void(APIENTRY *)(GLenum target, std::ptrdiff_t size, const void* data, GLenum usage);
    using GlDeleteBuffersFn = void(APIENTRY *)(GLsizei n, const GLuint* buffers);
    using GlEnableVertexAttribArrayFn = void(APIENTRY *)(GLuint index);
    using GlVertexAttribPointerFn = void(APIENTRY *)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
    using GlDrawArraysFn = void(APIENTRY *)(GLenum mode, GLint first, GLsizei count);

    GlCreateShaderFn CreateShader = nullptr;
    GlShaderSourceFn ShaderSource = nullptr;
    GlCompileShaderFn CompileShader = nullptr;
    GlGetShaderivFn GetShaderiv = nullptr;
    GlGetShaderInfoLogFn GetShaderInfoLog = nullptr;
    GlCreateProgramFn CreateProgram = nullptr;
    GlAttachShaderFn AttachShader = nullptr;
    GlLinkProgramFn LinkProgram = nullptr;
    GlGetProgramivFn GetProgramiv = nullptr;
    GlGetProgramInfoLogFn GetProgramInfoLog = nullptr;
    GlDeleteShaderFn DeleteShader = nullptr;
    GlDeleteProgramFn DeleteProgram = nullptr;
    GlUseProgramFn UseProgram = nullptr;
    GlGetUniformLocationFn GetUniformLocation = nullptr;
    GlGetAttribLocationFn GetAttribLocation = nullptr;
    GlUniform1iFn Uniform1i = nullptr;
    GlUniform2iFn Uniform2i = nullptr;
    GlUniform4fFn Uniform4f = nullptr;
    GlActiveTextureFn ActiveTexture = nullptr;
    GlGenFramebuffersFn GenFramebuffers = nullptr;
    GlBindFramebufferFn BindFramebuffer = nullptr;
    GlFramebufferTexture2DFn FramebufferTexture2D = nullptr;
    GlCheckFramebufferStatusFn CheckFramebufferStatus = nullptr;
    GlDeleteFramebuffersFn DeleteFramebuffers = nullptr;
    GlGenVertexArraysFn GenVertexArrays = nullptr;
    GlBindVertexArrayFn BindVertexArray = nullptr;
    GlDeleteVertexArraysFn DeleteVertexArrays = nullptr;
    GlGenBuffersFn GenBuffers = nullptr;
    GlBindBufferFn BindBuffer = nullptr;
    GlBufferDataFn BufferData = nullptr;
    GlDeleteBuffersFn DeleteBuffers = nullptr;
    GlEnableVertexAttribArrayFn EnableVertexAttribArray = nullptr;
    GlVertexAttribPointerFn VertexAttribPointer = nullptr;
    GlDrawArraysFn DrawArrays = nullptr;

    bool Load() {
        if (loaded) {
            return true;
        }

        auto load = [](const char* name) -> void* {
            return reinterpret_cast<void*>(glfwGetProcAddress(name));
        };

        CreateShader = reinterpret_cast<GlCreateShaderFn>(load("glCreateShader"));
        ShaderSource = reinterpret_cast<GlShaderSourceFn>(load("glShaderSource"));
        CompileShader = reinterpret_cast<GlCompileShaderFn>(load("glCompileShader"));
        GetShaderiv = reinterpret_cast<GlGetShaderivFn>(load("glGetShaderiv"));
        GetShaderInfoLog = reinterpret_cast<GlGetShaderInfoLogFn>(load("glGetShaderInfoLog"));
        CreateProgram = reinterpret_cast<GlCreateProgramFn>(load("glCreateProgram"));
        AttachShader = reinterpret_cast<GlAttachShaderFn>(load("glAttachShader"));
        LinkProgram = reinterpret_cast<GlLinkProgramFn>(load("glLinkProgram"));
        GetProgramiv = reinterpret_cast<GlGetProgramivFn>(load("glGetProgramiv"));
        GetProgramInfoLog = reinterpret_cast<GlGetProgramInfoLogFn>(load("glGetProgramInfoLog"));
        DeleteShader = reinterpret_cast<GlDeleteShaderFn>(load("glDeleteShader"));
        DeleteProgram = reinterpret_cast<GlDeleteProgramFn>(load("glDeleteProgram"));
        UseProgram = reinterpret_cast<GlUseProgramFn>(load("glUseProgram"));
        GetUniformLocation = reinterpret_cast<GlGetUniformLocationFn>(load("glGetUniformLocation"));
        GetAttribLocation = reinterpret_cast<GlGetAttribLocationFn>(load("glGetAttribLocation"));
        Uniform1i = reinterpret_cast<GlUniform1iFn>(load("glUniform1i"));
        Uniform2i = reinterpret_cast<GlUniform2iFn>(load("glUniform2i"));
        Uniform4f = reinterpret_cast<GlUniform4fFn>(load("glUniform4f"));
        ActiveTexture = reinterpret_cast<GlActiveTextureFn>(load("glActiveTexture"));
        GenFramebuffers = reinterpret_cast<GlGenFramebuffersFn>(load("glGenFramebuffers"));
        BindFramebuffer = reinterpret_cast<GlBindFramebufferFn>(load("glBindFramebuffer"));
        FramebufferTexture2D = reinterpret_cast<GlFramebufferTexture2DFn>(load("glFramebufferTexture2D"));
        CheckFramebufferStatus = reinterpret_cast<GlCheckFramebufferStatusFn>(load("glCheckFramebufferStatus"));
        DeleteFramebuffers = reinterpret_cast<GlDeleteFramebuffersFn>(load("glDeleteFramebuffers"));
        GenVertexArrays = reinterpret_cast<GlGenVertexArraysFn>(load("glGenVertexArrays"));
        BindVertexArray = reinterpret_cast<GlBindVertexArrayFn>(load("glBindVertexArray"));
        DeleteVertexArrays = reinterpret_cast<GlDeleteVertexArraysFn>(load("glDeleteVertexArrays"));
        GenBuffers = reinterpret_cast<GlGenBuffersFn>(load("glGenBuffers"));
        BindBuffer = reinterpret_cast<GlBindBufferFn>(load("glBindBuffer"));
        BufferData = reinterpret_cast<GlBufferDataFn>(load("glBufferData"));
        DeleteBuffers = reinterpret_cast<GlDeleteBuffersFn>(load("glDeleteBuffers"));
        EnableVertexAttribArray = reinterpret_cast<GlEnableVertexAttribArrayFn>(load("glEnableVertexAttribArray"));
        VertexAttribPointer = reinterpret_cast<GlVertexAttribPointerFn>(load("glVertexAttribPointer"));
        DrawArrays = reinterpret_cast<GlDrawArraysFn>(load("glDrawArrays"));

        loaded =
            CreateShader && ShaderSource && CompileShader && GetShaderiv &&
            GetShaderInfoLog && CreateProgram && AttachShader && LinkProgram &&
            GetProgramiv && GetProgramInfoLog && DeleteShader && DeleteProgram &&
            UseProgram && GetUniformLocation && GetAttribLocation && Uniform1i &&
            Uniform2i && Uniform4f && ActiveTexture && GenFramebuffers &&
            BindFramebuffer && FramebufferTexture2D && CheckFramebufferStatus && DeleteFramebuffers &&
            GenVertexArrays && BindVertexArray && DeleteVertexArrays && GenBuffers && BindBuffer &&
            BufferData && DeleteBuffers && EnableVertexAttribArray && VertexAttribPointer && DrawArrays;

        if (!loaded) {
            std::cerr << "Failed to load OpenGL functions for GPU spectrogram rendering\n";
        }

        return loaded;
    }

private:
    bool loaded = false;
};

OpenGLFunctions g_gl;

} // namespace

namespace GUI::Window::Helper {

SpectrogramRenderer::~SpectrogramRenderer() {
    Shutdown();
}

bool SpectrogramRenderer::Init() {
    if (m_initialized) {
        return true;
    }

    if (!g_gl.Load() || !CreateProgram()) {
        return false;
    }

    const float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };

    g_gl.GenVertexArrays(1, &m_quadVao);
    g_gl.BindVertexArray(m_quadVao);
    g_gl.GenBuffers(1, &m_quadVbo);
    g_gl.BindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
    g_gl.BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLint positionLocation = g_gl.GetAttribLocation(m_shaderProgram, "aPosition");
    if (positionLocation < 0) {
        std::cerr << "Spectrogram shader is missing aPosition attribute\n";
        return false;
    }

    g_gl.EnableVertexAttribArray(static_cast<GLuint>(positionLocation));
    g_gl.VertexAttribPointer(static_cast<GLuint>(positionLocation), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    g_gl.BindBuffer(GL_ARRAY_BUFFER, 0);
    g_gl.BindVertexArray(0);

    g_gl.GenFramebuffers(1, &m_framebuffer);
    m_initialized = true;
    return true;
}

void SpectrogramRenderer::Resize(int width, int height) {
    m_width = width;
    m_height = height;

    if (m_spectrogramTex) {
        glDeleteTextures(1, &m_spectrogramTex);
        m_spectrogramTex = 0;
    }
    if (m_magnitudeTex) {
        glDeleteTextures(1, &m_magnitudeTex);
        m_magnitudeTex = 0;
    }

    if (m_width <= 0 || m_height <= 0) {
        return;
    }

    glGenTextures(1, &m_spectrogramTex);
    glBindTexture(GL_TEXTURE_2D, m_spectrogramTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &m_magnitudeTex);
    glBindTexture(GL_TEXTURE_2D, m_magnitudeTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
                 m_width, m_height, 0,
                 GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLint previousFramebuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    g_gl.BindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    g_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_spectrogramTex, 0);
    if (g_gl.CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Spectrogram framebuffer is incomplete\n";
    }
    g_gl.BindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFramebuffer));
}

void SpectrogramRenderer::PushColumn(const std::vector<float>& magnitudes, int column) {
    if (!Init() || m_width <= 0 || m_height <= 0 || !m_magnitudeTex || !m_spectrogramTex) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_magnitudeTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, column, 0, 1, m_height,
                    GL_RED, GL_FLOAT, magnitudes.data());

    const int nextColumn = (column + 1) % m_width;
    RenderTexture(nextColumn);
}

void SpectrogramRenderer::Shutdown() {
    if (m_spectrogramTex) {
        glDeleteTextures(1, &m_spectrogramTex);
        m_spectrogramTex = 0;
    }
    if (m_magnitudeTex) {
        glDeleteTextures(1, &m_magnitudeTex);
        m_magnitudeTex = 0;
    }
    if (m_framebuffer && g_gl.DeleteFramebuffers) {
        g_gl.DeleteFramebuffers(1, &m_framebuffer);
        m_framebuffer = 0;
    }
    if (m_quadVbo && g_gl.DeleteBuffers) {
        g_gl.DeleteBuffers(1, &m_quadVbo);
        m_quadVbo = 0;
    }
    if (m_quadVao && g_gl.DeleteVertexArrays) {
        g_gl.DeleteVertexArrays(1, &m_quadVao);
        m_quadVao = 0;
    }
    if (m_shaderProgram && g_gl.DeleteProgram) {
        g_gl.DeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }

    m_initialized = false;
}

GLuint SpectrogramRenderer::TextureId() const {
    return m_spectrogramTex;
}

bool SpectrogramRenderer::CreateProgram() {
    const char* vertexShaderSource = R"(
#version 130
in vec2 aPosition;
out vec2 vUv;

void main()
{
    vUv = aPosition * 0.5 + 0.5;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
)";

    const char* fragmentShaderSource = R"(
#version 130
uniform sampler2D uMagnitudes;
uniform int uCurrentColumn;
uniform ivec2 uSize;
uniform vec4 uMidtone;
uniform vec4 uHighlight;

in vec2 vUv;
out vec4 fragColor;

void main()
{
    int displayX = clamp(int(floor(vUv.x * float(uSize.x))), 0, uSize.x - 1);
    int outputY = clamp(int(floor(vUv.y * float(uSize.y))), 0, uSize.y - 1);
    int y = uSize.y - outputY - 1;
    int sourceX = (displayX + uCurrentColumn) % uSize.x;
    float mag = clamp(texelFetch(uMagnitudes, ivec2(sourceX, y), 0).r * 2.0, 0.0, 1.0);

    if (mag < 0.5) {
        float t = mag / 0.5;
        fragColor = vec4(uMidtone.rgb, uMidtone.a * t);
    } else {
        float t = (mag - 0.5) / 0.5;
        fragColor = mix(uMidtone, uHighlight, t);
    }
}
)";

    auto compileShader = [&](GLenum type, const char* source) -> GLuint {
        GLuint shader = g_gl.CreateShader(type);
        g_gl.ShaderSource(shader, 1, &source, nullptr);
        g_gl.CompileShader(shader);

        GLint success = 0;
        g_gl.GetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024] = {};
            g_gl.GetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Failed to compile spectrogram shader: " << infoLog << "\n";
            g_gl.DeleteShader(shader);
            return 0;
        }

        return shader;
    };

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (!vertexShader || !fragmentShader) {
        return false;
    }

    m_shaderProgram = g_gl.CreateProgram();
    g_gl.AttachShader(m_shaderProgram, vertexShader);
    g_gl.AttachShader(m_shaderProgram, fragmentShader);
    g_gl.LinkProgram(m_shaderProgram);
    g_gl.DeleteShader(vertexShader);
    g_gl.DeleteShader(fragmentShader);

    GLint success = 0;
    g_gl.GetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024] = {};
        g_gl.GetProgramInfoLog(m_shaderProgram, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Failed to link spectrogram shader: " << infoLog << "\n";
        g_gl.DeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
        return false;
    }

    m_magnitudesUniform = g_gl.GetUniformLocation(m_shaderProgram, "uMagnitudes");
    m_currentColumnUniform = g_gl.GetUniformLocation(m_shaderProgram, "uCurrentColumn");
    m_sizeUniform = g_gl.GetUniformLocation(m_shaderProgram, "uSize");
    m_midtoneUniform = g_gl.GetUniformLocation(m_shaderProgram, "uMidtone");
    m_highlightUniform = g_gl.GetUniformLocation(m_shaderProgram, "uHighlight");

    return true;
}

void SpectrogramRenderer::RenderTexture(int currentColumn) {
    GLint previousFramebuffer = 0;
    GLint previousViewport[4] = {};
    GLint previousProgram = 0;
    GLint previousActiveTexture = 0;
    GLint previousTexture0 = 0;
    GLint previousVao = 0;
    GLint previousArrayBuffer = 0;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glGetIntegerv(GL_VIEWPORT, previousViewport);
    glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActiveTexture);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVao);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousArrayBuffer);
    g_gl.ActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture0);

    g_gl.BindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_width, m_height);
    g_gl.UseProgram(m_shaderProgram);
    g_gl.ActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_magnitudeTex);

    g_gl.Uniform1i(m_magnitudesUniform, 0);
    g_gl.Uniform1i(m_currentColumnUniform, currentColumn);
    g_gl.Uniform2i(m_sizeUniform, m_width, m_height);

    const auto& midtone = GUI::Constants::Colors::MIDTONE;
    const auto& highlight = GUI::Constants::Colors::HIGHLIGHT;
    g_gl.Uniform4f(m_midtoneUniform,
                   midtone[0] / 255.0f, midtone[1] / 255.0f, midtone[2] / 255.0f, midtone[3] / 255.0f);
    g_gl.Uniform4f(m_highlightUniform,
                   highlight[0] / 255.0f, highlight[1] / 255.0f, highlight[2] / 255.0f, highlight[3] / 255.0f);

    g_gl.BindVertexArray(m_quadVao);
    g_gl.DrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    g_gl.BindVertexArray(static_cast<GLuint>(previousVao));
    g_gl.BindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(previousArrayBuffer));
    g_gl.ActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture0));
    g_gl.ActiveTexture(static_cast<GLenum>(previousActiveTexture));
    g_gl.UseProgram(static_cast<GLuint>(previousProgram));
    g_gl.BindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFramebuffer));
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
}

} // namespace GUI::Window::Helper
