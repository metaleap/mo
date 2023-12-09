#include <cassert>
#include <vector>

#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Window/Event.hpp>

#include "./gui.h"


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
}

void Gui::onRender() {
    ImGui::SFML::Render(this->window);
}

void Gui::onShutDown() {
    ImGui::SFML::Shutdown();
}
