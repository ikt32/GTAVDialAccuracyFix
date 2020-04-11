#include "Script.h"

#include "Settings.h"
#include "Memory/VehicleExtensions.h"

#include "Util/Logger.hpp"
#include "Util/Math.hpp"

#include <DashHook/DashHook.h>
#include <ScriptHookV_SDK/inc/natives.h>
#include <ScriptHookV_SDK/inc/main.h>

#include <string>
#include <cstdlib>

class SpeedoFixScript {
    Settings mSettings;

    Vehicle mVehicle;
    Hash mModel;
    // for lerping
    float mLastRPM;
public:
    SpeedoFixScript(std::string settingsFile)
        : mSettings(std::move(settingsFile))
        , mVehicle(0), mModel(0), mLastRPM(0) {
        mSettings.Load();
    }

    void Tick() {
        updateCheatMonitor();
        updatePlayerVehicle();

        if (mModel)
            updateDashboardData();
    }

private:
    void updateCheatMonitor() {
        if (GAMEPLAY::_HAS_CHEAT_STRING_JUST_BEEN_ENTERED(
            GAMEPLAY::GET_HASH_KEY(mSettings.Main.Cheat.c_str()))) {
            mSettings.Load();
        }
    }

    void updatePlayerVehicle() {
        mVehicle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
        if (ENTITY::DOES_ENTITY_EXIST(mVehicle)) {
            mModel = ENTITY::GET_ENTITY_MODEL(mVehicle);
        }
        else {
            mVehicle = 0;
            mModel = 0;
        }
    }

    void updateDashboardData() {
        auto cfgIt = mSettings.Configs.find(mModel);
        if (cfgIt == mSettings.Configs.end()) {
            return;
        }

        //float rpm;
        //if (!VEHICLE::GET_IS_VEHICLE_ENGINE_RUNNING(mVehicle)) {
        //    rpm = 0.0f;
        //}
        //else {
        //    rpm = VehicleExtensions::GetRPM(mVehicle);
        //}

        //float rpmLerp = Math::Lerp(mLastRPM, rpm, 
        //    1.0f - pow(0.0001f, GAMEPLAY::GET_FRAME_TIME()));
        //mLastRPM = rpmLerp;
        //
        //// Bounce the rev limiter, ignore lerping
        //if (rpmLerp >= 0.99f) {
        //    // bounce +/- 0.05 RPM
        //    float randFloat = (static_cast<float>(rand()) / static_cast <float> (RAND_MAX) - 0.5f) * 0.05f;
        //    rpmLerp = rpm + randFloat;
        //}

        // Game uses mph dials, so scale based on mph.
        float dashSpeedMs = VehicleExtensions::GetDashSpeed(mVehicle);
        float dashSpeedMph = dashSpeedMs / 0.44704f;

        VehicleDashboardData data;
        DashHook_GetData(&data);
        data.speed = cfgIt->second.SpeedOffset + 
            dashSpeedMph * cfgIt->second.SpeedScale;
        data.RPM = cfgIt->second.RPMOffset +
            data.RPM * cfgIt->second.RPMScale;
        DashHook_SetData(data);
    }
};

void ScriptMain() {
    srand(GetTickCount());
    logger.SetMinLevel(DEBUG);
    VehicleExtensions::Init();

    SpeedoFixScript script("SpeedoFix/settings.ini");
    
    while (true) {
        script.Tick();
        WAIT(0);
    }
}
