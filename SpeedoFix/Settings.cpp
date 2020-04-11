#include "Settings.h"

#include "Util/Logger.hpp"

#include <simpleini/SimpleIni.h>
#include <stdexcept>

#define CHECK_LOG_SI_ERROR(result, operation) \
    if ((result) < 0) { \
        logger.Write(ERROR, "[Settings] %s Failed to %s, SI_Error [%d]", \
        __FUNCTION__, operation, result); \
    }

namespace Util {
    constexpr unsigned long joaat(const char* s) {
        unsigned long hash = 0;
        for (; *s != '\0'; ++s) {
            auto c = *s;
            if (c >= 0x41 && c <= 0x5a) {
                c += 0x20;
            }
            hash += c;
            hash += hash << 10;
            hash ^= hash >> 6;
        }
        hash += hash << 3;
        hash ^= hash >> 11;
        hash += hash << 15;
        return hash;
    }
}

Settings::Settings(std::string settingsFile)
    : mSettingsFile(std::move(settingsFile)) {
    
}

void Settings::Load() {
    logger.Write(INFO, "[Configs] Loading");
    Configs.clear();

    CSimpleIniA ini;
    ini.SetUnicode();
    SI_Error result = ini.LoadFile(mSettingsFile.c_str());
    CHECK_LOG_SI_ERROR(result, "load");

    // [MAIN]
    Main.Cheat = ini.GetValue("Main", "Cheat", Main.Cheat.c_str());

    // Load all configs
    // Layout:
    //
    // [<modelname>]
    // SpeedScale = <float>
    // ; In MPH
    // SpeedOffset = <float>
    //
    // RPMScale = <float>
    // RPMOffset = <float>

    CSimpleIniA::TNamesDepend sections;
    ini.GetAllSections(sections);

    for (const auto& section : sections) {
        float speedScale = static_cast<float>(ini.GetDoubleValue(section.pItem, "SpeedScale", 1.0f));
        float speedOffset = static_cast<float>(ini.GetDoubleValue(section.pItem, "SpeedOffset", 0.0f));
        float rpmScale = static_cast<float>(ini.GetDoubleValue(section.pItem, "RPMScale", 1.0f));
        float rpmOffset = static_cast<float>(ini.GetDoubleValue(section.pItem, "RPMOffset", 0.0f));

        if (speedScale == 1.0f && speedOffset == 0.0f &&
            rpmScale == 1.0f && rpmOffset == 0.0f) {
            continue;
        }

        Configs[Util::joaat(section.pItem)] = { speedScale, speedOffset, rpmScale, rpmOffset };
    }
}
