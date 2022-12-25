#pragma once
#include <string>
#include <functional>
#include <random>
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_stdlib.h"
#include "IMGUI/imgui_searchablecombo.h"

// Components wrap ImGui widgets and bind them to callbacks for when they're clicked/typed in
namespace ImGuiComponents {

    template<typename ReturnSig, typename ... Args>
    class AbstractComponent {
        public:
            explicit                         AbstractComponent(std::string name, std::function<ReturnSig(Args...)> on_interact_callback) : name(std::move(name)), id(GenerateId()), on_interact_callback(std::move(on_interact_callback)){}
            virtual                          ~AbstractComponent() = default;
            virtual void                     Render(){}
            [[nodiscard]] const std::string& GetName() const { return name; }
            [[nodiscard]] const std::string& GetId() const { return id; }
            void                             SetName(const std::string& name) { this->name = name; id = GenerateId(); }
            [[nodiscard]] const float&       GetWidth() const { return width; }
            void                             SetWidth(const float& new_width) { width = new_width; }
            virtual void                     ResizeWidthWithWindow() { width = -1.0f * CalculateLabelWidth(); }
            virtual float                    CalculateLabelWidth() {
                const auto font = ImGui::GetFont();
                const auto size = font->Scale;
                if(!font) return -100;
                float width = 0.0f;
                for(const auto& character : name) {
                    std::string temp;
                    temp += character;
                    for(const auto& glyph : font->Glyphs) {
                        if(std::wstring(temp.begin(), temp.end())[0] == glyph.Codepoint) {
                            width += glyph.AdvanceX;
                            break;
                        }
                    }
                }
                return width * size + ImGui::GetStyle().FramePadding.x;
            }

        protected:
            bool active_flag = false;
            std::string name, id; // name = text displayed on UI; id = text displayed on UI + random ID sequence
            std::function<ReturnSig(Args...)> on_interact_callback;
            float width = 0.0f;
            bool pushed_width = false;

            virtual void SizeRuleBegin(){
                if(width != 0.0f) {
                    ImGui::PushItemWidth(width);
                    pushed_width = true;
                }
            }

            virtual void SizeRuleEnd(){
                if(pushed_width) {
                    ImGui::PopItemWidth();
                    pushed_width = false;
                }
            }
        private:
            // Guarantees uniqueness of name to prevent name collisions within ImGui
            std::string GenerateId(){
                std::string id_section = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
                std::random_device rd;
                std::mt19937_64 gen(rd());
                std::ranges::shuffle(id_section, gen);
                return name + "##" + id_section.substr(0,8);
            }
    };

    class Text : public AbstractComponent<void> {
        public:
            explicit Text(const std::string& name, const ImVec4& color = {255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f});
            ~Text() override = default;
            void Render() override;
            [[nodiscard]] ImVec4 GetColor() { return {color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w}; }

            // RGBA format; 0-255 RGB, 0-1 A
            void SetColor(const ImVec4& new_color) { color.x = new_color.x / 255.0f; color.y = new_color.y / 255.0f; color.z = new_color.z / 255.0f; color.w = new_color.w; }
            void SetNameAndColor(const std::string& new_name, const ImVec4& new_color) { SetColor(new_color); name = new_name; }
        protected:
            ImVec4 color;
    };

    class Checkbox : public AbstractComponent<void, const bool&> {
        public:
            explicit Checkbox(const std::string& name, const bool& checked = false, std::function<void(const bool&)> on_interact_callback = {});
            void Render() override;
            ~Checkbox() override = default;
            [[nodiscard]] bool GetChecked() { return checked; }
            void SetChecked(const bool& checked) { this->checked = checked; }
            
        private:
            void SizeRuleBegin() override{}
            void SizeRuleEnd() override{}
            void ResizeWidthWithWindow() override {}
            float CalculateLabelWidth() override { return 1.0f; }
            bool checked = false;
    };

    class Combobox : public AbstractComponent<void, int, const std::vector<std::string>&> {
        public:
            explicit Combobox(const std::string& name, std::vector<std::string> options, std::function<void(int, const std::vector<std::string>&)> on_interact_callback = {});
            void     Render() override;
            ~Combobox() override = default;
            [[nodiscard]] std::string GetSelected();
            [[nodiscard]] int GetSelectedIndex() { return selected; }
            void SetSelectedIndex(const int& to_select) { selected = to_select; }
        protected:
            std::vector<std::string> options;
            int selected = 0;
    };

    class Button : public AbstractComponent<void> {
        public:
            explicit Button(const std::string& name, std::function<void()> on_interact_callback = {});
            ~Button() override = default;
            void Render() override;
            void ResizeWidthWithWindow() override { width = -FLT_MIN; resize_prop = 1.0f; }
            void ResizeWidthWithWindowProportional(const float& prop) { resize_prop = prop * -1.0f; width = 0.0f; }

        private:
            float resize_prop = 1.0f;
            void SizeRuleBegin() override;
    };

    class InputText : public AbstractComponent<void, const std::string*> {
        public:
            explicit InputText(const std::string& name, std::function<void(const std::string*)> on_interact_callback = {});
            ~InputText() override = default;
            void                              Render() override;
            [[nodiscard]] bool                GetInputEnabled();
            void                              SetInputEnabled(const bool& is_enabled);
            void                              ClearInput();
            void                              SetText(const std::string& text) { input_buffer = text; }
            [[nodiscard]] const std::string&  GetText() { return input_buffer; }
            void                              SetFlags(const int& flags) { this->flags = flags; }
            [[nodiscard]] ImGuiInputTextFlags GetFlags() { return flags; }
        protected:
            bool input_enabled = true;
            std::string input_buffer;
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
    };

    class MultilineInputText : public InputText {
        public:
            explicit MultilineInputText(const std::string& name, std::function<void(const std::string*)> on_interact_callback = {});
            ~MultilineInputText() override = default;
            void Render() override;
    };

    // WARNING: These color constants are only valid for these widgets, and may not produce the correct result if used with traditional ImGui methods
    namespace ColorConstants {
        extern const ImVec4 RED;
        extern const ImVec4 GREEN;
        extern const ImVec4 BLUE;
        extern const ImVec4 WHITE;
        extern const ImVec4 BLACK;
        extern const ImVec4 YELLOW;
    };
}

