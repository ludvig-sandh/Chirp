// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include "audio/preset/AudioPreset.hpp"


class BuiltInPresetsLoader {
public:
    static BuiltInPresetsLoader& GetShared() {
        static BuiltInPresetsLoader instance;
        return instance;
    }

    // Return list of preset names
    std::vector<std::string>& GetPresetNames();
    
    // Returns the index of the name in the list of preset names that corresponds to the default preset.
    // Returns -1 if it wasn't found.
    int GetIndexOfDefaultPreset();

    // Load a preset by name (adds ".json" automatically)
    bool LoadBuiltInPreset(AudioPreset& preset, const std::string& name) const;

    // Loads the "chirp" preset
    bool LoadDefaultPreset(AudioPreset& preset) const;

private:
    // Reads preset names once and saves them internally in the singleton object
    BuiltInPresetsLoader();

    // Return preset folder path (cross-platform safe)
    static std::filesystem::path GetPresetsFolder();
    static inline const std::filesystem::path PRESETS_DIR = PRESETS_DIR_PATH; // Provided by CML
    static inline const std::string DEFAULT_PRESET_NAME = "chirp";

    std::vector<std::string> m_presetNames;
    std::filesystem::path m_builtInPresetsFolder;
};
