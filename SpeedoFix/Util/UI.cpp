#include "UI.hpp"

#include <ScriptHookV_SDK/inc/natives.h>
#include <string>
#include "../Constants.hpp"

namespace {
    int notificationHandle = 0;

    void notify(const std::string& message, int* prevNotification) {
        if (prevNotification != nullptr && *prevNotification != 0) {
            UI::_REMOVE_NOTIFICATION(*prevNotification);
        }
        UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");

        UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(message.c_str());

        int id = UI::_DRAW_NOTIFICATION(false, false);
        if (prevNotification != nullptr) {
            *prevNotification = id;
        }
    }
}

void UI::Notify(const std::string& message) {
    Notify(message, true);
}

void UI::Notify(const std::string& message, bool removePrevious) {
    int* notifHandleAddr = nullptr;
    if (removePrevious) {
        notifHandleAddr = &notificationHandle;
    }
    ::notify(std::string(Constants::NotificationPrefix) + "\n" + message, notifHandleAddr);
}