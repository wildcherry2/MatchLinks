#include "pch.h"
#include "Menu.h"
#include "NamesMap.h"
#include "MatchLinks.h"
#include "Settings.h"

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
    create_button->Render();
    ImGui::SameLine();
    join_button->Render();
    ImGui::SameLine();
    copy_join_link_button->Render();
    ImGui::SameLine();
    copy_create_link_button->Render();
    ImGui::NewLine();
    status_text->Render();
    warning_text->Render();
}

bool Menu::CheckName() {
    if(!match_data.name || match_data.name->empty()){
        status_text->SetNameAndColor(NameRequiredError.msg, NameRequiredError.color);
        return false;
    }

    return true;
}

bool Menu::CheckPassword() {
    const bool pass_valid = match_data.password;
    const bool pass_entered = pass_valid ? !match_data.password->empty() : false;
    if(!pass_valid || !pass_entered) {
        warning_text->SetNameAndColor(PassSuggestedWarning.msg, PassSuggestedWarning.color);
        return false;
    }

    warning_text->SetName("");
    return true;
    
}

void Menu::CopyToClipboard(const std::string& text) {
    ImGui::LogToClipboard();
    ImGui::LogText(text.c_str());
    ImGui::LogFinish();
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

    //region_combo->ResizeWidthWithWindow();

    auto& settings = Settings::Instance().GetSettingsData();

    region_combo->SetSelectedIndex(settings.region);
    int map = 0; // if map isn't found for some reason, defaults to first option by initializing it here
    auto& selected_map_name = settings.map;
    for(int it = 0; it < internal_map_names.size(); it++) {
        if(internal_map_names[it] == selected_map_name) {
            map = it;
            break;
        }
    }
    map_combo->SetSelectedIndex(map);
    //map_combo->ResizeWidthWithWindow();

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
    //password_input->ResizeWidthWithWindow();

    copy_join_link_button = std::make_shared<Button>("Copy Join Link", [this] {
        try {
            if(!CheckName()) return;

            CheckPassword();

            const std::string ss = std::vformat(R"(http://localhost:{}/match?action=join&name={}&password={})", std::make_format_args(Settings::Instance().GetSettingsData().port, *match_data.name, match_data.password ? *match_data.password : ""));
            CopyToClipboard(ss);

            status_text->SetNameAndColor(CopiedJoinLink.msg, CopiedJoinLink.color);
        }
        catch(...) {
            LOG("Internal formatting error!");
            status_text->SetNameAndColor(FormattingError.msg, FormattingError.color);
        }
    });

    copy_create_link_button = std::make_shared<Button>("Copy Create Link", [this] {
        try {
            if(!CheckName()) return;

            CheckPassword();

            const std::string ss = std::vformat(R"(http://localhost:{}/match?action=create&name={}&password={}&region={})",
                                          std::make_format_args(Settings::Instance().GetSettingsData().port, *match_data.name, match_data.password ? *match_data.password : "", static_cast<int>(match_data.region)));
            CopyToClipboard(ss);

            status_text->SetNameAndColor(CopiedCreateLink.msg, CopiedCreateLink.color);
        }
        catch(...){
            LOG("Internal formatting error!");
            status_text->SetNameAndColor(FormattingError.msg, FormattingError.color);
        }
    });

    reset_button = std::make_shared<Button>("Reset", [this] {
        *match_data.name = "";
        *match_data.password = "";
        name_input->ClearInput();
        password_input->ClearInput();
    });

    create_button = std::make_shared<Button>("Create Match", [this] {
        plugin->gameWrapper->Execute([this](GameWrapper* gw) {
                auto mmwrapper = gw->GetMatchmakingWrapper();
                if(!CheckName()) return;
                if(!mmwrapper.IsNull()) {
                    CustomMatchSettings cms;
                    CustomMatchTeamSettings blue, orange;
                    const bool pass_exists = CheckPassword();

                    cms.ServerName = *match_data.name;
                    cms.Password = pass_exists ? *match_data.password : "";
                    cms.MapName = *match_data.map;
                    cms.GameTags = "BotsNone";
                    cms.BlueTeamSettings = blue;
                    cms.OrangeTeamSettings = orange;
                    cms.GameMode = 0;
                    cms.bClubServer = false;
                    cms.bPartyMembersOnly = false;

                    LOG("Creating lobby with name = {}, password = {}, region = {}, map = {}", cms.ServerName, cms.Password, static_cast<int>(match_data.region), *match_data.map);

                    mmwrapper.CreatePrivateMatch(match_data.region, cms);
                }
            });
    });

    join_button = std::make_shared<Button>("Join Match", [this] {
        plugin->gameWrapper->Execute([this](GameWrapper* gw) {
            auto mmwrapper = gw->GetMatchmakingWrapper();
            if(!CheckName()) return;
            if(!mmwrapper.IsNull()) {
                const auto& pass = CheckPassword() ? *match_data.password : "";
                LOG("Joining lobby with name = {} and password = {}", *match_data.name, pass);
                mmwrapper.JoinPrivateMatch(*match_data.name, pass);
            }
        });
    });

    status_text = std::make_shared<Text>("", ImGuiComponents::ColorConstants::WHITE);
    warning_text = std::make_shared<Text>("", ImGuiComponents::ColorConstants::WHITE);
    
    match_data.map = &settings.map;
    match_data.region = static_cast<Region>(settings.region);

    //name_input->ResizeWidthWithWindow();
    //password_input->ResizeWidthWithWindow();
}
