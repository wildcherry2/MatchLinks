#pragma once
#include "Widgets.h"
#include "NamesMap.h"
#include "MatchLinks.h"

using ImGuiComponents::Combobox;
using ImGuiComponents::Checkbox;
using ImGuiComponents::Button;
using ImGuiComponents::InputText;

struct MatchData;

struct SettingsData{
    bool enabled = true;
    int region = 0;
    std::string map = internal_map_names[0];
    uint16_t port = 2525;
    //bool remember_previous_info = true;
    MatchData* previous_info = nullptr;
};

class Settings {
    public:
        static Settings& Instance();
        bool Save();
        bool Load();
        void Render();
        [[nodiscard]] SettingsData& GetSettingsData() { return data; }
    private:
        Settings();
        ~Settings();
        std::filesystem::path json_path;
        std::ifstream reader;
        std::ofstream writer;
        std::shared_ptr<Combobox> settings_region_combobox;
        std::shared_ptr<Combobox> settings_map_combobox;
        std::shared_ptr<Button> settings_save_button;
        std::shared_ptr<Checkbox> enabled_checkbox;
        //std::shared_ptr<Checkbox> remember_last_info_checkbox;
        std::shared_ptr<InputText> port_inputtext;
        SettingsData data;
};
