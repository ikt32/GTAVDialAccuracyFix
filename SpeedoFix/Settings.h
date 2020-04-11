#pragma once

#include <ScriptHookV_SDK/inc/types.h>

#include <string>
#include <unordered_map>

struct Values {
    float SpeedScale;
    float SpeedOffset;
    float RPMScale;
    float RPMOffset;
};

class Settings {
public:
    Settings(std::string settingsFile);

    void Load();

    // [MAIN]
    struct {
        std::string Cheat = "reloadspeedo";
    } Main;

    // [CONFIGS]
    std::unordered_map<Hash, Values> Configs;

private:
    std::string mSettingsFile;
};
