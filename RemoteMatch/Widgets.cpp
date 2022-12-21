#include "pch.h"
#include "Widgets.h"

#pragma region AbstractComponent

ImGuiComponents::DefaultFunctorSig::DefaultFunctorSig(std::function<void()> default_callback) : default_callback(default_callback) {}

ImGuiComponents::AbstractComponent::AbstractComponent(const std::string& name, std::shared_ptr<DefaultFunctorSig> on_interact_callback)
    : name(name), id(GenerateId()), on_interact_callback(on_interact_callback) {}

std::string ImGuiComponents::AbstractComponent::GenerateId() {
    std::string id_section = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::ranges::shuffle(id_section, gen);
    return name + "##" + id_section.substr(0,8);
}

#pragma endregion

#pragma region Button

ImGuiComponents::Button::Button(const std::string& name, std::shared_ptr<DefaultFunctorSig> on_interact_callback) : AbstractComponent(name, std::move(on_interact_callback)) {}
ImGuiComponents::Button::Button(const std::string& name, std::function<void()> on_interact_callback) : AbstractComponent(name, std::make_shared<DefaultFunctorSig>(on_interact_callback)) {}

void ImGuiComponents::Button::Render() {
    if(ImGui::Button(id.c_str())) {
        (*on_interact_callback)();
    }
}

#pragma endregion

#pragma region InputText

ImGuiComponents::InputText::InputTextCallback::InputTextCallback(std::function<void(const std::string*)> input_text_callback) : DefaultFunctorSig(), input_text_callback(input_text_callback) {}

void ImGuiComponents::InputText::InputTextCallback::operator()() {
    input_text_callback(param_buffer);
}

ImGuiComponents::InputText::InputText(const std::string& name, std::shared_ptr<InputTextCallback> on_interact_callback) : AbstractComponent(name) {
    on_interact_callback->param_buffer = &input_buffer;
    this->on_interact_callback = on_interact_callback;
}

ImGuiComponents::InputText::InputText(const std::string& name, std::function<void(const std::string*)> on_interact_callback) : AbstractComponent(name) {
    auto functor = std::make_shared<InputTextCallback>(on_interact_callback);
    functor->param_buffer = &input_buffer;
    this->on_interact_callback = functor;
}

void ImGuiComponents::InputText::ClearInput() {
    input_buffer.clear();
}
void ImGuiComponents::InputText::Render() {
    if(ImGui::InputText(id.c_str(), &input_buffer, flags)) {
        (*on_interact_callback)();
    }
}
void ImGuiComponents::InputText::SetInputEnabled(const bool& is_enabled) { input_enabled = is_enabled; }
bool ImGuiComponents::InputText::GetInputEnabled() { return input_enabled; }

#pragma endregion

#pragma region MultilineInputText

ImGuiComponents::MultilineInputText::MultilineInputText(const std::string& name, std::shared_ptr<InputTextCallback> on_interact_callback) : InputText(name, std::move(on_interact_callback)){  }
ImGuiComponents::MultilineInputText::MultilineInputText(const std::string& name, std::function<void(const std::string*)> on_interact_callback) : InputText(name, on_interact_callback) {}
void ImGuiComponents::MultilineInputText::Render() {
    if(ImGui::InputTextMultiline(id.c_str(), &input_buffer, {0, 0}, input_enabled ? ImGuiInputTextFlags_None : ImGuiInputTextFlags_ReadOnly)) {
        (*on_interact_callback)();
    }
}

#pragma endregion

#pragma region Combobox

ImGuiComponents::Combobox::Combobox(const std::string& name, std::vector<std::string> options, std::function<void(int, const std::vector<std::string>&)> on_interact_callback) : AbstractComponent(name),
                                                                                                                                                                                 options(std::move(options)),
                                                                                                                                                                                 on_interact_callback(on_interact_callback) {
    
}

void ImGuiComponents::Combobox::Render() {
    if(ImGui::SearchableCombo(id.c_str(), &selected, options, "","")) {
        on_interact_callback(selected, options);
    }
}

std::string ImGuiComponents::Combobox::GetSelected() {
    return options[selected];
}

#pragma endregion

#pragma region Checkbox

ImGuiComponents::Checkbox::Checkbox(const std::string& name, const bool& checked, std::function<void(const bool&)> on_interact_callback) : AbstractComponent(name), on_interact_callback(on_interact_callback), checked(checked) {}

void ImGuiComponents::Checkbox::Render() {
    if(ImGui::Checkbox(id.c_str(), &checked)){
        on_interact_callback(checked);
    }
}

#pragma endregion
