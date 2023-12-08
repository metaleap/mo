#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

struct AppView {
    virtual ~AppView();

  public:
    virtual void onUpdate(sf::Time delta);
    virtual void onRender(sf::RenderWindow &window);
};
