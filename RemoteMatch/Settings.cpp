#include "pch.h"
#include "Settings.h"
#include "Server.h"
using nlohmann::json;

Settings& Settings::Instance() {
    static Settings settings;
    return settings;
}

bool Settings::Save() {
    try {
        writer.open(json_path, std::ios::trunc);
        json out_settings;
        out_settings["enabled"] = data.enabled;
        out_settings["region"] = data.region;
        out_settings["map"] = data.map;
        out_settings["port"] = data.port;
        //out_settings["remember_previous_info"] = data.remember_previous_info;
        writer << out_settings;
        writer.close();

        LOG("Settings saved to {}", json_path.string());
    }

    catch(const std::exception& ex){
        LOG("Error saving settings! {}", ex.what());
        return false;
    }

    return true;
}

bool Settings::Load() {
    try {
        reader.open(json_path);
        json in_settings;
        reader >> in_settings;
        reader.close();

        data.enabled = in_settings["enabled"];
        data.region = in_settings["region"];
        data.map = in_settings["map"];
        //data.remember_previous_info = in_settings["remember_previous_info"];
        data.port = in_settings["port"];
    }

    catch(const std::exception& ex){
        LOG("Error loading settings! {}", ex.what());
        return false;
    }
    return true;
}

void Settings::Render() {
    enabled_checkbox->Render();

    if(data.enabled) {
        //remember_last_info_checkbox->Render();
        settings_map_combobox->Render();
        settings_region_combobox->Render();
        port_inputtext->Render();
        //settings_save_button->Render();
    }
}

#define CVAR(subscriber, converter, ...) plugin->cvarManager->registerCvar(__VA_ARGS__).addOnValueChanged([this](const std::string& old_val, CVarWrapper cvar){ \
    data.##subscriber = cvar.##converter ();\
    Save();\
})\

#define SETVAR(name, value) plugin->cvarManager->getCvar(name).setValue(value)

Settings::Settings() {
    json_path = plugin->gameWrapper->GetDataFolder() / "MatchLinks.json";

    Load();

    CVAR(enabled, getBoolValue, "ml_enabled", data.enabled ? "1" : "0", "", true, true, 0, true, 1, false);
    //CVAR(remember_previous_info, getBoolValue, "ml_remember_info", data.remember_previous_info ? "1" : "0", "", true, true, 0, true, 1, false);
    CVAR(region, getIntValue, "ml_region", std::to_string(data.region), "", true, true, 0, true, 10, false);
    CVAR(map, getStringValue, "ml_map", data.map, "", true, false, 0, false, 0, false);
    CVAR(port, getIntValue, "ml_port", std::to_string(data.port), "", true, true, 0, true, UINT16_MAX, false);

    enabled_checkbox = std::make_shared<Checkbox>("Enable", data.enabled, [this](const bool& checked){
        SETVAR("ml_enabled", checked ? "1" : "0");
        auto& server = ServerListener::Instance();
        if(!checked && server.IsListening())
            ServerListener::Instance().StopServer();
        else if(checked && !server.IsListening())
            ServerListener::Instance().StartServer();
    });

    /*remember_last_info_checkbox = std::make_shared<Checkbox>("Remember previous match information", data.remember_previous_info, [this](const bool& checked){
        SETVAR("ml_remember_info", checked ? "1" : "0");
    });*/

    port_inputtext = std::make_shared<InputText>("Listen Port (0-65535)",[this](const std::string* in){
        if(in) {
            try {
                auto new_port = std::stoull(*in);
                if(in->find("-") != std::string::npos) {
                    LOG("Error setting port! Port number can't be negative!");
                    // TOOLTIP: UINT ONLY!
                }
                else if(new_port <= UINT16_MAX) {
                    SETVAR("ml_port", *in);
                    ServerListener::Instance().SetPort(new_port);
                }
                else {
                    LOG("Error setting port! Port number can only be up to 65,535!");
                    // TOOLTIP: 2 BYTES ONLY!
                }
            }

            catch(const std::exception& ex) {
                LOG("Invalid port entry: {}, {}", *in, ex.what());
                // TOOLTIP: BAD PORT VALUE
            }
        }
    });

    port_inputtext->SetText(std::to_string(data.port));
    port_inputtext->SetFlags(ImGuiInputTextFlags_CharsDecimal);

    settings_map_combobox = std::make_shared<Combobox>("Default Map", friendly_map_names, [this](int index, const std::vector<std::string>& options){
        SETVAR("ml_map", internal_map_names[index]);
    });

    settings_region_combobox = std::make_shared<Combobox>("Default Region", region_friendly_names, [this](int index, const std::vector<std::string>& options){
        SETVAR("ml_region", static_cast<int>(region_map.at(options[index])));
    });

    settings_region_combobox->SetSelectedIndex(data.region);
    int map = 0; // if map isn't found for some reason, defaults to first option by initializing it here
    auto& selected_map_name = data.map;
    for(int it = 0; it < internal_map_names.size(); it++) {
        if(internal_map_names[it] == selected_map_name) {
            map = it;
            break;
        }
    }
    settings_map_combobox->SetSelectedIndex(map);

    /*settings_save_button = std::make_shared<Button>("Save Settings", [this]{
        Save();
    });*/
}

#undef CVAR
#undef SETVAR

#define UNREG(name) plugin->cvarManager->removeCvar(name)

Settings::~Settings() {
    Save();
    UNREG("ml_enabled");
    UNREG("ml_map");
    UNREG("ml_region");
    //UNREG("ml_remember_info");
    UNREG("ml_port");
}

#undef UNREG