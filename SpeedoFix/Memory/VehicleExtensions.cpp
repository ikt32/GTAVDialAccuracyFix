#include "VehicleExtensions.h"

#include "../Util/Logger.hpp"

#include <Windows.h>
#include <Psapi.h>

unsigned VehicleExtensions::sDashSpeedOffset = 0;
unsigned VehicleExtensions::sRPMOffset = 0;

namespace Memory {
    uintptr_t(*GetAddressOfEntity)(int entity) = nullptr;

    uintptr_t FindPattern(const char* pattern, const char* mask) {
        MODULEINFO modInfo = { nullptr };
        GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &modInfo, sizeof(MODULEINFO));

        const char* start_offset = reinterpret_cast<const char*>(modInfo.lpBaseOfDll);
        const uintptr_t size = static_cast<uintptr_t>(modInfo.SizeOfImage);

        intptr_t pos = 0;
        const uintptr_t searchLen = static_cast<uintptr_t>(strlen(mask) - 1);

        for (const char* retAddress = start_offset; retAddress < start_offset + size; retAddress++) {
            if (*retAddress == pattern[pos] || mask[pos] == '?') {
                if (mask[pos + 1] == '\0')
                    return (reinterpret_cast<uintptr_t>(retAddress) - searchLen);
                pos++;
            }
            else {
                pos = 0;
            }
        }
        return 0;
    }

    void Init() {
        auto addr = FindPattern("\x83\xF9\xFF\x74\x31\x4C\x8B\x0D\x00\x00\x00\x00\x44\x8B\xC1\x49\x8B\x41\x08",
            "xxxxxxxx????xxxxxxx");
        if (!addr)
            logger.Write(ERROR, "Couldn't find GetAddressOfEntity");
        GetAddressOfEntity = reinterpret_cast<uintptr_t(*)(int)>(addr);
    }
}

void VehicleExtensions::Init() {
    Memory::Init();

    auto addr = Memory::FindPattern("\xF3\x0F\x10\x8F\x10\x0A\x00\x00\xF3\x0F\x59\x05\x5E\x30\x8D\x00",
                                 "xxxx????xxxx????");
    sDashSpeedOffset = addr == 0 ? 0 : *reinterpret_cast<unsigned*>(addr + 4);
    logger.Write(sDashSpeedOffset == 0 ? WARN : DEBUG, "Dashboard Speed Offset: 0x%X", sDashSpeedOffset);

    addr = Memory::FindPattern("\x76\x03\x0F\x28\xF0\xF3\x44\x0F\x10\x93",
        "xxxxxxxxxx");
    sRPMOffset = addr == 0 ? 0 : *reinterpret_cast<unsigned*>(addr + 10);
    logger.Write(sRPMOffset == 0 ? WARN : DEBUG, "RPM Offset: 0x%X", sRPMOffset);
}

float VehicleExtensions::GetDashSpeed(Vehicle handle) {
    if (sDashSpeedOffset == 0)
        return 0;
    auto address = Memory::GetAddressOfEntity(handle);
    return *reinterpret_cast<float*>(address + sDashSpeedOffset);
}

float VehicleExtensions::GetRPM(Vehicle handle) {
    if (sRPMOffset == 0)
        return 0;
    auto address = Memory::GetAddressOfEntity(handle);
    return *reinterpret_cast<float*>(address + sRPMOffset);
}
