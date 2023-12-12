#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>

#include "../appview.h"


struct LiveView : AppView {
    LiveView();

    void onUpdate(const sf::Time &delta);
    void onRender(sf::RenderWindow &window);
    void onInput(const sf::Event &evt);

    void setupAndLoadResources();

    sf::Texture bgTexture;
    sf::RectangleShape rect;
};
