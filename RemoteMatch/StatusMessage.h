#pragma once
#include "Widgets.h"

struct StatusMessage {
    std::string msg;
    ImVec4 color;

    static inline const ImVec4& GetGoodColor() { return ImGuiComponents::ColorConstants::GREEN; }
    static inline const ImVec4& GetWarningColor() { return ImGuiComponents::ColorConstants::YELLOW; }
    static inline const ImVec4& GetErrorColor() { return ImGuiComponents::ColorConstants::RED; }
};
