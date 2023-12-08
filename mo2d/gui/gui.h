#pragma once

#include <vector>

#include <imgui.h>
#include <SFML/Graphics.hpp>

#include "../appviews/appview.h"

struct Gui {
  public:
    sf::RenderWindow &window;
    ImFont* font = nullptr;

    Gui(sf::RenderWindow &win, std::vector<AppView> views);
    void onInput(const sf::Event evt);
    void onUpdate(const sf::Time delta);
    void onRender();
    void onShutDown();
};
