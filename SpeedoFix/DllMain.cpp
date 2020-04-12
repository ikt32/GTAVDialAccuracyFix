#include "Script.h"

#include "Constants.hpp"
#include "Util/Logger.hpp"

#include <ScriptHookV_SDK/inc/main.h>
#include <Psapi.h>

#include <filesystem>

namespace fs = std::filesystem;

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved) {
    const std::string modPath = "DialAccuracyFix";
    const std::string logFile = modPath + "\\" + Constants::Name + ".log";

    if (!fs::is_directory(modPath) || !fs::exists(modPath)) {
        fs::create_directory(modPath);
    }

    logger.SetFile(logFile);

    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            logger.Clear();
            logger.Write(LogLevel::INFO, "DialAccuracyFix %s (built %s %s)", Constants::DisplayVersion, __DATE__, __TIME__);

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
