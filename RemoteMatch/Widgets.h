#pragma once
#include <string>
#include <functional>
#include <random>
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_stdlib.h"
#include "IMGUI/imgui_searchablecombo.h"

// Components wrap ImGui widgets and bind them to callbacks for when they're clicked/typed in
namespace ImGuiComponents {

    // [DEPRECATED] Can't really use templates/std::bind since we want a central manager to manage and render all widgets regardless of their callbacks, so polymorphism is the best option
    struct DefaultFunctorSig {
        DefaultFunctorSig() = default;
        DefaultFunctorSig(std::function<void()> default_callback);
        virtual      ~DefaultFunctorSig() = default;
        virtual void operator()() {
            default_callback();
        }

        private:
            std::function<void()> default_callback;
    };

    class AbstractComponent {
        public:
            explicit AbstractComponent(const std::string& name, std::shared_ptr<DefaultFunctorSig> on_interact_callback = nullptr);
            virtual      ~AbstractComponent() = default;
            virtual void Render(){}
            const std::string& GetName() { return name; }
            const std::string& GetId() { return id; }
            void SetName(const std::string& name) { this->name = name; id = GenerateId(); }
            
        protected:
            bool active_flag = false;
            std::string name, id;
            std::shared_ptr<DefaultFunctorSig> on_interact_callback = nullptr;
        private:
            // Guarantees uniqueness of name to prevent name collisions within ImGui
            std::string GenerateId();
    };

    class Checkbox : public AbstractComponent {
        public:
            explicit Checkbox(const std::string& name, const bool& checked = false, std::function<void(const bool&)> on_interact_callback = {});
            void Render() override;
            ~Checkbox() override = default;
            bool GetChecked() { return checked; }
            void SetChecked(const bool& checked) { this->checked = checked; }
        private:
            bool checked = false;
            std::function<void(const bool&)> on_interact_callback;
    };

    class Combobox : public AbstractComponent {
        public:
            explicit Combobox(const std::string& name, std::vector<std::string> options, std::function<void(int, const std::vector<std::string>&)> on_interact_callback = {});
            void     Render() override;
            ~Combobox() override = default;
            std::string GetSelected();
            [[nodiscard]] int GetSelectedIndex() { return selected; }
        protected:
            std::vector<std::string> options;
            int selected = 0;
            std::function<void(int, const std::vector<std::string>&)> on_interact_callback;
    };

    class Button : public AbstractComponent {
        public:
            explicit Button(const std::string& name, std::shared_ptr<DefaultFunctorSig> on_interact_callback = nullptr);
            explicit Button(const std::string& name, std::function<void()> on_interact_callback = {});
            ~Button() override = default;
            void Render() override;
    };

    class InputText : public AbstractComponent {
        public:
            // Non-default function binding for InputText widgets
            struct InputTextCallback : public DefaultFunctorSig {
                InputTextCallback() = default;
                explicit InputTextCallback(std::function<void(const std::string*)> input_text_callback);
                ~InputTextCallback() override = default;
                void operator()() override;
                std::string* param_buffer = nullptr;

                private:
                    std::function<void(const std::string*)> input_text_callback;
            };

            explicit InputText(const std::string& name, std::shared_ptr<InputTextCallback> on_interact_callback = nullptr);
            explicit InputText(const std::string& name, std::function<void(const std::string*)> on_interact_callback = {});
            ~InputText() override = default;
            void Render() override;
            bool GetInputEnabled();
            void SetInputEnabled(const bool& is_enabled);
            void ClearInput();
            void SetText(const std::string& text) { input_buffer = text; }
            const std::string& GetText() { return input_buffer; }
            void SetFlags(int flags) { this->flags = flags; }
            ImGuiInputTextFlags GetFlags() { return flags; }
        protected:
            std::shared_ptr<InputTextCallback> on_interact_callback;
            bool input_enabled = true;
            std::string input_buffer;
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
    };

    class MultilineInputText : public InputText {
        public:
            explicit MultilineInputText(const std::string& name, std::shared_ptr<InputTextCallback> on_interact_callback);
            explicit MultilineInputText(const std::string& name, std::function<void(const std::string*)> on_interact_callback = {});
            ~MultilineInputText() override = default;
            void Render() override;
    };

}