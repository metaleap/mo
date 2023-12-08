#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

struct AppView {
    virtual ~AppView() {
    }

    virtual void onUpdate(sf::Time delta) = 0;
    virtual void onRender(sf::RenderWindow &window) = 0;
};
