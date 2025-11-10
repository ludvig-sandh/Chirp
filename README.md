# 🎵 Chirp
A cross-platform real-time modular synthesizer written in modern C++ with ImGui and PortAudio.

![Build](https://github.com/ludvig-sandh/Chirp/actions/workflows/build.yml/badge.svg)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![Platform](https://img.shields.io/badge/Platforms-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)

![Chirp screenshot](assets/screenshot.png)


## Build Instructions

### Requirements
- CMake ≥ 3.15
- C++20 compiler
- Git

### Clone and build
```bash
git clone --recursive https://github.com/ludvig-sandh/Chirp.git
cd Chirp
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel