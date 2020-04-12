#pragma once

#include <string>

namespace UI {
    void Notify(const std::string& message);
    void Notify(const std::string& message, bool removePrevious);
}