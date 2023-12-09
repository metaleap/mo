#pragma once

#include <vector>

#include <imgui.h>

#include "../appviews/appview.h"


struct Gui {
    sf::RenderWindow &window;
    std::vector<AppView*> views;
    ImFont* font = nullptr;

    Gui(sf::RenderWindow &win, std::vector<AppView*> views);
    void onInput(const sf::Event evt);
    void onUpdate(const sf::Time delta);
    void onRender();
    void onShutDown();
};
