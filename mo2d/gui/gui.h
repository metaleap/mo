#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>

struct Gui {
  public:
    sf::RenderWindow &window;
    ImFont* font = nullptr;

    Gui(sf::RenderWindow &win);
    void processEvent(const sf::Event evt);
    void update(const sf::Time delta);
    void render();
    void onShutDown();
};
