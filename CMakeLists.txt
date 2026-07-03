cmake_minimum_required(VERSION 3.15)
project(chirp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# ===========================
# PortAudio (submodule under src/external/portaudio)
# ===========================
#
set(PA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/external/portaudio)
set(PA_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/portaudio-build)

set(PA_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PA_BUILD_STATIC ON  CACHE BOOL "" FORCE)

add_subdirectory(${PA_SRC_DIR} ${PA_BUILD_DIR})

# Detect actual PortAudio target name (static vs shared)
set(PORTAUDIO_TGT portaudio)
if (TARGET portaudio_static)
    set(PORTAUDIO_TGT portaudio_static)
endif()

# Suppress ALSA deprecation warnings inside PortAudio only
if (TARGET ${PORTAUDIO_TGT} AND NOT MSVC)
    # safe on GCC/Clang only
    target_compile_options(${PORTAUDIO_TGT} PRIVATE
        -Wno-deprecated-declarations
    )
endif()

#
# Collect only the core sources (not src/external)
#
file(GLOB CHIRP_CORE_SRCS CONFIGURE_DEPENDS
    src/*.cpp
    src/audio/*.cpp
    src/audio/core/*.cpp
    src/audio/effects/*.cpp
    src/audio/effects/dsp/*.cpp
    src/audio/engine/*.cpp
    src/audio/generator/*.cpp
    src/audio/layout/*.cpp
    src/audio/modulation/*.cpp
    src/audio/preset/*.cpp
    src/fft/*.cpp
    src/gui/*.cpp
    src/gui/windows/*.cpp
    src/gui/windows/helper/*.cpp
    src/synchronization/*.cpp
    src/midi/*.cpp
    src/util/*.cpp
)

#
# ImGui sources (match your Makefile exactly)
#
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/external/imgui)

set(IMGUI_SRCS
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

#
# ImGuiFileDialog source (must be built)
#
set(IMGUI_FILE_DIALOG ${CMAKE_CURRENT_SOURCE_DIR}/src/external/ImGuiFileDialog/ImGuiFileDialog.cpp)

#
# Final source list
#
set(SRC_FILES
    ${CHIRP_CORE_SRCS}
    ${IMGUI_SRCS}
    ${IMGUI_FILE_DIALOG}
)

#
# Create executable *before* linking or adding include dirs
#
add_executable(chirp ${SRC_FILES})

# Copy presets directory to the executable output folder after build
add_custom_command(TARGET chirp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/presets"
    "$<TARGET_FILE_DIR:chirp>/presets"
)

#
# PortAudio include paths (suppress warnings)
#
target_include_directories(chirp SYSTEM PRIVATE
    ${PA_SRC_DIR}/include
    ${PA_BUILD_DIR}
)

#
# User include directories
#
target_include_directories(chirp PRIVATE
    src
    src/external
    src/external/imgui
    src/external/imgui/backends
    src/external/ImGuiFileDialog
    src/external/pocketfft
)

#
# ===========================
# GLFW
# ===========================
#
include(FetchContent)

# Fetch GLFW (static)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        latest
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

target_link_libraries(chirp PRIVATE glfw)

#
# ===========================
# RtMidi (for cross-platform MIDI input)
# ===========================
#
FetchContent_Declare(
    rtmidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG 88de822e8ffcba28167e30d287671ccb64f88637
)

set(RTMIDI_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(RTMIDI_BUILD_SHARED OFF CACHE BOOL "" FORCE)

# Only apply defaults on Linux (UNIX but not macOS)
if(UNIX AND NOT APPLE)
    # Default to ALSA ON and JACK OFF, but allow user override
    set(RTMIDI_API_ALSA ON CACHE BOOL "Enable ALSA backend")
    set(RTMIDI_API_JACK OFF CACHE BOOL "Enable JACK backend")
else()
    # Let RtMidi choose defaults on other systems
endif()

FetchContent_MakeAvailable(rtmidi)
target_link_libraries(chirp PRIVATE rtmidi)

#
# Link PortAudio
#
target_link_libraries(chirp PRIVATE ${PORTAUDIO_TGT})

#
# ===========================
# Platform-specific OpenGL & system libs
# ===========================
#
if (WIN32)
    target_link_libraries(chirp PRIVATE
        opengl32
        gdi32
        imm32
        winmm
        ole32
        uuid
    )
elseif (APPLE)
    target_link_libraries(chirp PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )

    # Since macOS now wants to use metal rather than opengl, we suppress the warnings about this
    target_compile_definitions(chirp PRIVATE GL_SILENCE_DEPRECATION)
elseif (UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(chirp PRIVATE OpenGL::GL pthread dl)
endif()
