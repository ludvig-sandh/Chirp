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
```

> **Note:**  
> If CMake reports an error such as  
> `-- Building for: NMake Makefiles`  
> or  
> `CMAKE_CXX_COMPILER not set, after EnableLanguage`,  
> it means no compiler toolchain was detected automatically.  
> In that case, specify one explicitly when configuring:
>
> - **MSVC (Visual Studio)**  
>   ```bash
>   cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
>   ```
>
> - **MinGW (GCC)**  
>   ```bash
>   cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
>   ```
>
> - **Ninja**  
>   ```bash
>   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
>   ```
>
> Most Linux and macOS environments detect a compiler automatically,  
> but on Windows, CMake may default to **NMake**, which requires a Visual Studio Developer Prompt.

### Run without building

If you don’t want to compile Chirp yourself, you can also download a **prebuilt binary** directly from GitHub:

1. Go to the **[Actions](../../actions)** tab of this repository.  
2. Click the latest workflow run (with a green ✅ “Build” badge).  
3. Scroll down to the **Artifacts** section at the bottom.  
4. Download the ZIP file for your platform:
   - `chirp-windows-latest.zip`
   - `chirp-ubuntu-latest.zip`
   - `chirp-macos-latest.zip`
5. Extract it and run the executable (`chirp.exe` on Windows, `chirp` on Linux/macOS).

> **Note:** 
> These binaries are automatically built and tested on every commit using GitHub Actions,  
> so they’re always up to date with the latest source.
