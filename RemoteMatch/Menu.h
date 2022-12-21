#pragma once
#include "Widgets.h"

using ImGuiComponents::Combobox;
using ImGuiComponents::InputText;
using ImGuiComponents::Button;

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
        std::shared_ptr<Button> copy_and_create_match_button;

        Menu();
        MatchData match_data;
};