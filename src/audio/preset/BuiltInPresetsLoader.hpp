// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include "preset/AudioPreset.hpp"


class BuiltInPresetsLoader {
public:
    static BuiltInPresetsLoader& GetShared() {
        static BuiltInPresetsLoader instance;
        return instance;
    }

    // Return list of preset names
    std::vector<std::string>& GetPresetNames();

    // Load a preset by name (adds ".json" automatically)
    bool LoadBuiltInPreset(AudioPreset& preset, const std::string& name) const;

private:
    // Reads preset names once and saves them internally in the singleton object
    BuiltInPresetsLoader();

    // Return preset folder path (cross-platform safe)
    static std::filesystem::path GetPresetsFolder();
    static inline const std::filesystem::path PATH_FROM_EXE_TO_PRESETS =
        std::filesystem::path("..") / "presets";

    std::vector<std::string> m_presetNames;
    std::filesystem::path m_builtInPresetsFolder;
};
