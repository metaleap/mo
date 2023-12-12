#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

struct AppView {
    virtual void onInput(const sf::Event &evt) = 0;
    virtual void onUpdate(const sf::Time &delta) = 0;
    virtual void onRender(sf::RenderWindow &window) = 0;
};
