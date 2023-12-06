#pragma once

#include "imgui.h"
#include <SFML/Graphics.hpp>

struct Gui {
  public:
    sf::RenderWindow &window;
    ImFont* font = nullptr;

    Gui(sf::RenderWindow &win);
    void onInput(const sf::Event evt);
    void onUpdate(const sf::Time delta);
    void onRender();
    void onShutDown();
};
