#include "pch.h"
#include "Menu.h"
#include "NamesMap.h"
#include "RemoteMatch.h"

Menu& Menu::Instance() {
    static Menu menu;
    return menu;
}

void Menu::Render() {
    name_input->Render();
    password_input->Render();
    reset_button->Render();
    ImGui::SameLine();
    show_password_button->Render();
    ImGui::NewLine();
    map_combo->Render();
    region_combo->Render();
    copy_and_create_match_button->Render();
}

Menu::Menu() {
    name_input = std::make_shared<InputText>("Name", [this](const std::string* selected) {
        match_data.name = const_cast<std::string*>(selected);
    });

    password_input = std::make_shared<InputText>("Password", [this](const std::string* selected) {
        match_data.password = const_cast<std::string*>(selected);
    });

    map_combo = std::make_shared<Combobox>("Map", friendly_map_names, [this](int selected, const std::vector<std::string>& options) {
        match_data.map = const_cast<std::string*>(&internal_map_names[selected]);
    });

    region_combo = std::make_shared<Combobox>("Region", region_friendly_names, [this](int selected, const std::vector<std::string>& options) {
        match_data.region = region_map.at(options[selected]);
    });

    show_password_button = std::make_shared<Button>("Show Password", [this] {
        if(show_password_button->GetName() == "Show Password") {
            password_input->SetFlags(ImGuiInputTextFlags_None);
            show_password_button->SetName("Hide Password");
        }
        else {
            password_input->SetFlags(ImGuiInputTextFlags_Password);
            show_password_button->SetName("Show Password");
        }
    });

    password_input->SetFlags(ImGuiInputTextFlags_Password);

    copy_and_create_match_button = std::make_shared<Button>("Copy Link and Create Match", [this] {
        try {
            std::stringstream ss;
            ss << std::vformat(R"(http://localhost:2525/match?action=join?name={}?password={})", std::make_format_args(*match_data.name, *match_data.password));
            ImGui::LogToClipboard();
            ImGui::LogText(ss.str().c_str());
            ImGui::LogFinish();

            plugin->gameWrapper->Execute([this](GameWrapper* gw) {
                auto mmwrapper = gw->GetMatchmakingWrapper();
                if(!mmwrapper.IsNull()) {
                    CustomMatchSettings cms;
                    CustomMatchTeamSettings blue, orange;

                    cms.ServerName = *match_data.name;
                    cms.Password = *match_data.password;
                    cms.MapName = *match_data.map;
                    cms.GameTags = "BotsNone";
                    cms.BlueTeamSettings = blue;
                    cms.OrangeTeamSettings = orange;
                    cms.GameMode = 0;
                    cms.bClubServer = false;
                    cms.bPartyMembersOnly = false;

                    mmwrapper.CreatePrivateMatch(match_data.region, cms);
                }
            });
        }
        catch(...) {
            LOG("Internal formatting error!");
        }
    });

    reset_button = std::make_shared<Button>("Reset", [this] {
        *match_data.name = "";
        *match_data.password = "";
        name_input->ClearInput();
        password_input->ClearInput();
    });
}
