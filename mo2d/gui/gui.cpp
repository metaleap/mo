#include <SFML/Window/Event.hpp>
#include <cassert>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>

#include "./gui.h"

Gui::Gui(sf::RenderWindow &win) : window(win) {
    assert(ImGui::SFML::Init(this->window, false));
    ImGuiIO &imgui_io = ImGui::GetIO();
    imgui_io.IniFilename = "/home/_/.config/mo2d_dbg.config";
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    this->font = imgui_io.Fonts->AddFontDefault();
    assert(ImGui::SFML::UpdateFontTexture());
}

void Gui::processEvent(const sf::Event evt) {
    ImGui::SFML::ProcessEvent(this->window, evt);
}

void Gui::update(const sf::Time delta) {
    ImGui::SFML::Update(window, delta);
    ImGui::PushFont(font);
    ImGui::ShowDemoWindow();
    ImGui::ShowUserGuide();
    ImGui::Begin("Hello, world!");
    if (ImGui::Button("Look at this pretty button"))
        ImGui::Text("You clicked");
    ImGui::End();
    ImGui::PopFont();
}

void Gui::render() {
    ImGui::SFML::Render(this->window);
}

void Gui::onShutDown() {
    ImGui::SFML::Shutdown();
}
