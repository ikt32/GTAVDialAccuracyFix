#include "Script.h"

#include "Util/Logger.hpp"

#include <ScriptHookV_SDK/inc/main.h>
#include <Psapi.h>

#include <filesystem>

namespace fs = std::filesystem;

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved) {
    const std::string modPath = "SpeedoFix";
    const std::string logFile = modPath + "\\SpeedoFix.log";

    if (!fs::is_directory(modPath) || !fs::exists(modPath)) {
        fs::create_directory(modPath);
    }

    logger.SetFile(logFile);

    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            logger.Clear();
            logger.Write(LogLevel::INFO, "SpeedoFix %s (built %s %s)", "v1.0.0", __DATE__, __TIME__);

            scriptRegister(hInstance, ScriptMain);
            logger.Write(LogLevel::INFO, "Script registered");
            break;
        }
        case DLL_PROCESS_DETACH: {            
            scriptUnregister(hInstance);
            break;
        }
        default:
            // Do nothing
            break;
    }
    return TRUE;
}
