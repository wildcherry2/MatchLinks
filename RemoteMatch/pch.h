#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "GuiBase.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <fstream>
#include <set>

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_stdlib.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_rangeslider.h"

#include "Simple-Web-Server/server_http.hpp"

#include "json/single_include/nlohmann/json.hpp"

#include "logging.h"