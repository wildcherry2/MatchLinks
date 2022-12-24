#pragma once
#include "Widgets.h"

using ImGuiComponents::Combobox;
using ImGuiComponents::InputText;
using ImGuiComponents::Button;
using ImGuiComponents::Text;

struct MatchData {
    std::string* name = nullptr;
    std::string* password = nullptr;
    std::string* map = nullptr;
    Region region = Region::USE;
};

class Menu {
    public:
        static Menu& Instance();
        void Render();
        MatchData& GetMatchData() { return match_data; }
    private:
        std::shared_ptr<InputText> name_input;
        std::shared_ptr<InputText> password_input;
        std::shared_ptr<Button> show_password_button;
        std::shared_ptr<Button> reset_button;
        std::shared_ptr<Combobox> map_combo;
        std::shared_ptr<Combobox> region_combo;
        std::shared_ptr<Button> copy_join_link_button;
        std::shared_ptr<Button> copy_create_link_button;
        std::shared_ptr<Button> join_button;
        std::shared_ptr<Button> create_button;
        std::shared_ptr<Text> status_text;
        std::shared_ptr<Text> warning_text;

        struct StatusMessage {
            std::string msg;
            ImVec4 color;

            static inline const ImVec4& GetGoodColor() { return ImGuiComponents::ColorConstants::GREEN; }
            static inline const ImVec4& GetWarningColor() { return ImGuiComponents::ColorConstants::YELLOW; }
            static inline const ImVec4& GetErrorColor() { return ImGuiComponents::ColorConstants::RED; }
        };

        static inline const StatusMessage NameRequiredError = { "ERROR: A name is required for private matches!", StatusMessage::GetErrorColor() };
        static inline const StatusMessage PassSuggestedWarning = { "WARNING: This private match has no password set! Players may join with only the name.", StatusMessage::GetWarningColor() };
        static inline const StatusMessage FormattingError = { "ERROR: Error building the URL! Make sure the match settings are valid.", StatusMessage::GetErrorColor() };
        static inline const StatusMessage CopiedJoinLink = { "Copied join link to clipboard!", StatusMessage::GetGoodColor() };
        static inline const StatusMessage CopiedCreateLink = { "Copied create link to clipboard!", StatusMessage::GetGoodColor() };

        Menu();
        MatchData match_data;
};