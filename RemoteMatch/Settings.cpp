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
        writer << out_settings;
        writer.close();

        LOG("Settings saved to {}", json_path.string());
        status_text->SetNameAndColor(SaveSuccessful.msg, SaveSuccessful.color);
    }

    catch(const std::exception& ex){
        LOG("Error saving settings! {}", ex.what());
        status_text->SetNameAndColor(SaveError.msg, SaveError.color);
        return false;
    }

    return true;
}

bool Settings::Load() {
    try {
        if(std::filesystem::exists(json_path)) {
            reader.open(json_path);
            json in_settings;
            reader >> in_settings;
            reader.close();

            data.enabled = in_settings["enabled"];
            data.region = in_settings["region"];
            data.map = in_settings["map"];
            data.port = in_settings["port"];
            status_text->SetNameAndColor(LoadSuccessful.msg, LoadSuccessful.color);
        }
        else {
            Save(); // Save new cfg json if it doesn't exist yet
        }
    }

    catch(const std::exception& ex){
        LOG("Error loading settings! {}", ex.what());
        status_text->SetNameAndColor(LoadWarning.msg, LoadWarning.color);
        data.enabled = true;
        data.region = 0;
        data.map = internal_map_names[0];
        data.port = 2525;
        Save();
        return false;
    }
    return true;
}

void Settings::Render() {
    enabled_checkbox->Render();

    if(data.enabled) {
        settings_map_combobox->Render();
        settings_region_combobox->Render();
        port_inputtext->Render();
        status_text->Render();
    }
}

#define CVAR(subscriber, converter, ...) plugin->cvarManager->registerCvar(__VA_ARGS__).addOnValueChanged([this](const std::string& old_val, CVarWrapper cvar){ \
    data.##subscriber = cvar.##converter ();\
    Save();\
})\

#define SETVAR(name, value) plugin->cvarManager->getCvar(name).setValue(value)

Settings::Settings() {
    json_path = plugin->gameWrapper->GetDataFolder() / "MatchLinks.json";
    status_text = std::make_shared<Text>("");
    Load();

    CVAR(enabled, getBoolValue, "ml_enabled", data.enabled ? "1" : "0", "", true, true, 0, true, 1, false);
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

    port_inputtext = std::make_shared<InputText>("Listen Port (0-65535)",[this](const std::string* in){
        if(in) {
            try {
                const auto new_port = std::stoull(*in);
                if(in->find("-") != std::string::npos) {
                    LOG("Error setting port! Port number can't be negative!");
                    status_text->SetNameAndColor(SetPortError.msg, SetPortError.color);
                }
                else if(new_port <= UINT16_MAX) {
                    SETVAR("ml_port", *in);
                    ServerListener::Instance().SetPort(new_port);  // NOLINT(clang-diagnostic-implicit-int-conversion)
                    status_text->SetName("");
                }
                else {
                    LOG("Error setting port! Port number can only be up to 65,535!");
                    status_text->SetNameAndColor(SetPortError.msg, SetPortError.color);
                }
            }

            catch(const std::exception& ex) {
                LOG("Invalid port entry: {}, {}", *in, ex.what());
                status_text->SetNameAndColor(SetPortError.msg + "EXCEPTION CAUGHT: " + ex.what(), SetPortError.color);
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

    
}

#undef CVAR
#undef SETVAR

#define UNREG(name) plugin->cvarManager->removeCvar(name)

Settings::~Settings() {
    Save();
    UNREG("ml_enabled");
    UNREG("ml_map");
    UNREG("ml_region");
    UNREG("ml_port");
}

#undef UNREG