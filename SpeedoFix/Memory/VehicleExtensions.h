#pragma once

#include <ScriptHookV_SDK/inc/types.h>

class VehicleExtensions {
    static unsigned sDashSpeedOffset;
    static unsigned sRPMOffset;

public:
    static void Init();

    static float GetDashSpeed(Vehicle handle);
    static float GetRPM(Vehicle handle);
};