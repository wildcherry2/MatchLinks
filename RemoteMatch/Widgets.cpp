#include "pch.h"
#include "Widgets.h"

#pragma region Button

ImGuiComponents::Button::Button(const std::string& name, std::function<void()> on_interact_callback) : AbstractComponent(name, on_interact_callback) {}

void ImGuiComponents::Button::Render() {
    SizeRuleBegin();

    if(ImGui::Button(id.c_str())) {
        on_interact_callback();
    }

    SizeRuleEnd();
}

void ImGuiComponents::Button::SizeRuleBegin() {
    if(width != 0.0f) {
        ImGui::PushItemWidth(width);
        pushed_width = true;
    }
    else if(resize_prop != 1.0f) {
        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * resize_prop);
        pushed_width = true;
    }
}

#pragma endregion

#pragma region InputText

ImGuiComponents::InputText::InputText(const std::string& name, std::function<void(const std::string*)> on_interact_callback) : AbstractComponent<void, const std::string*>(name, on_interact_callback) {}

void ImGuiComponents::InputText::ClearInput() {
    input_buffer.clear();
}
void ImGuiComponents::InputText::Render() {
    SizeRuleBegin();

    if(ImGui::InputText(id.c_str(), &input_buffer, flags)) {
        on_interact_callback(&input_buffer);
    }

    SizeRuleEnd();
}

void ImGuiComponents::InputText::SetInputEnabled(const bool& is_enabled) {
    input_enabled = is_enabled;
    flags = is_enabled ? flags | ImGuiInputTextFlags_ReadOnly : flags & ~ImGuiInputTextFlags_ReadOnly;
}

bool ImGuiComponents::InputText::GetInputEnabled() { return input_enabled; }

#pragma endregion

#pragma region MultilineInputText

ImGuiComponents::MultilineInputText::MultilineInputText(const std::string& name, std::function<void(const std::string*)> on_interact_callback) : InputText(name, on_interact_callback) {}
void ImGuiComponents::MultilineInputText::Render() {
    SizeRuleBegin();

    if(ImGui::InputTextMultiline(id.c_str(), &input_buffer, {0, 0}, flags)) {
        on_interact_callback(&input_buffer);
    }

    SizeRuleEnd();
}

#pragma endregion

#pragma region Combobox

ImGuiComponents::Combobox::Combobox(const std::string& name, std::vector<std::string> options, std::function<void(int, const std::vector<std::string>&)> on_interact_callback) : AbstractComponent<void, int, const std::vector<std::string>&>(name, on_interact_callback),
                                                                                                                                                                                 options(std::move(options)){
    
}

void ImGuiComponents::Combobox::Render() {
    SizeRuleBegin();

    if(ImGui::SearchableCombo(id.c_str(), &selected, options, "","")) {
        on_interact_callback(selected, options);
    }

    SizeRuleEnd();
}

std::string ImGuiComponents::Combobox::GetSelected() {
    return options[selected];
}

#pragma endregion

#pragma region Checkbox

ImGuiComponents::Checkbox::Checkbox(const std::string& name, const bool& checked, std::function<void(const bool&)> on_interact_callback) : AbstractComponent<void, const bool&>(name, on_interact_callback), checked(checked) {}

void ImGuiComponents::Checkbox::Render() {
    SizeRuleBegin();

    if(ImGui::Checkbox(id.c_str(), &checked)){
        on_interact_callback(checked);
    }

    SizeRuleEnd();
}

#pragma endregion

#pragma region Text

ImGuiComponents::Text::Text(const std::string& name, const ImVec4& color) : AbstractComponent<void>(name, {}) {
    SetColor(color);
}

void ImGuiComponents::Text::Render() {
    ImGui::TextColored(color, name.c_str());
}

#pragma endregion


static const ImVec4 ImGuiComponents::ColorConstants::BLUE  = {0.0f, 0.0f, 255.0f, 1.0f};
static const ImVec4 ImGuiComponents::ColorConstants::RED   = {255.0f, 0.0f, 0.0f, 1.0f};
static const ImVec4 ImGuiComponents::ColorConstants::GREEN = {0.0f, 255.0f, 0.0f, 1.0f};
static const ImVec4 ImGuiComponents::ColorConstants::BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
static const ImVec4 ImGuiComponents::ColorConstants::WHITE = {255.0f, 255.0f, 255.0f, 1.0f};