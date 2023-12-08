#include <cassert>
#include <vector>

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>

#include "./gui.h"
#include "../appviews/appview.h"

Gui::Gui(sf::RenderWindow &win, std::vector<AppView*> views) : window(win), views(views) {
    assert(ImGui::SFML::Init(this->window, false));
    ImGuiIO &imgui_io = ImGui::GetIO();
    imgui_io.IniFilename = "/home/_/.config/mo2d_dbg.config";
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    this->font = imgui_io.Fonts->AddFontFromFileTTF("/home/_/f/Yanone Kaffeesatz/YanoneKaffeesatz-Regular.otf", 44);
    assert(ImGui::SFML::UpdateFontTexture());
}

void Gui::onInput(const sf::Event evt) {
    ImGui::SFML::ProcessEvent(this->window, evt);
}

void Gui::onUpdate(const sf::Time delta) {
    ImGui::SFML::Update(window, delta);
    ImGui::PushFont(font);
    ImGui::Begin("Hello, world!");
    if (ImGui::Button("Lookie a BUTT-on"))
        ImGui::Text("You clicked");
    ImGui::End();
    ImGui::PopFont();
}

void Gui::onRender() {
    ImGui::SFML::Render(this->window);
}

void Gui::onShutDown() {
    ImGui::SFML::Shutdown();
}
