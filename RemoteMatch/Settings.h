#pragma once
#include "Widgets.h"
#include "NamesMap.h"
#include "MatchLinks.h"
#include "StatusMessage.h"

using ImGuiComponents::Combobox;
using ImGuiComponents::Checkbox;
using ImGuiComponents::Button;
using ImGuiComponents::InputText;
using ImGuiComponents::Text;

struct MatchData;

struct SettingsData{
    bool enabled = true;
    int region = 0;
    std::string map = internal_map_names[0];
    uint16_t port = 2525;
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
        std::shared_ptr<Checkbox> enabled_checkbox;
        std::shared_ptr<Text> status_text;
        std::shared_ptr<InputText> port_inputtext;
        SettingsData data;

        static inline const StatusMessage SetPortError = { "ERROR: Port values must be between 0 and 65535!", StatusMessage::GetErrorColor() };
        static inline const StatusMessage LoadWarning = { "ERROR: Couldn't load saved settings! Assuming default...", StatusMessage::GetWarningColor() };
        static inline const StatusMessage SaveError = { "ERROR: Couldn't save settings!", StatusMessage::GetErrorColor() };
        static inline const StatusMessage LoadSuccessful = { "Loaded settings from config!", StatusMessage::GetGoodColor() };
        static inline const StatusMessage SaveSuccessful = { "Saved changes to config!", StatusMessage::GetGoodColor() };
};
