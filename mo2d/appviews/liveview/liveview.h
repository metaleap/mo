#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "../appview.h"


struct LiveView : AppView {
    LiveView();

    void onUpdate(sf::Time delta);
    void onRender(sf::RenderWindow &window);

    void setupAndLoadResources();

    sf::Texture bgTexture;
    sf::RectangleShape rect;
};
