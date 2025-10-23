#include "BuiltInPresetsLoader.hpp"
#include "AudioPresetSerialization.hpp"
#include <iostream>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <unistd.h>
#else
    #include <unistd.h>
#endif

// Cross platform code to find the path of the directory that directly contains the executable
static std::filesystem::path GetExecutableDir() {
#if defined(_WIN32)
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#elif defined(__APPLE__)
    char buffer[1024];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0)
        return std::filesystem::path(buffer).parent_path();
    else {
        std::vector<char> larger(size);
        _NSGetExecutablePath(larger.data(), &size);
        return std::filesystem::path(larger.data()).parent_path();
    }
#else // Linux / Unix
    char buffer[1024];
    ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (count != -1) {
        buffer[count] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }
    return std::filesystem::current_path();
#endif
}

std::vector<std::string>& BuiltInPresetsLoader::GetPresetNames() {
    return m_presetNames;
}

bool BuiltInPresetsLoader::LoadBuiltInPreset(AudioPreset& preset, const std::string& name) const {
    namespace fs = std::filesystem;

    fs::path presetPath = m_builtInPresetsFolder / (name + ".json");

    if (!fs::exists(presetPath)) {
        std::cerr << "Preset not found: " << presetPath << std::endl;
        return false;
    }

    return AudioPresetIO::LoadFromFile(preset, presetPath.string());
}

BuiltInPresetsLoader::BuiltInPresetsLoader()
    : m_builtInPresetsFolder(BuiltInPresetsLoader::GetPresetsFolder()) {

    namespace fs = std::filesystem;

    try {
        if (fs::exists(m_builtInPresetsFolder) && fs::is_directory(m_builtInPresetsFolder)) {
            for (const auto& entry : fs::directory_iterator(m_builtInPresetsFolder)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    m_presetNames.push_back(entry.path().stem().string()); // store name without extension
                }
            }
        }else {
            std::cerr << "Preset folder not found: " << m_builtInPresetsFolder << std::endl;
        }
    }catch (const std::exception& e) {
        std::cerr << "Error loading presets: " << e.what() << std::endl;
    }
}

std::filesystem::path BuiltInPresetsLoader::GetPresetsFolder() {
    // Always relative to executable
    std::filesystem::path exeDir = GetExecutableDir();
    std::filesystem::path presetDir = exeDir / PATH_FROM_EXE_TO_PRESETS;
    return presetDir;
}