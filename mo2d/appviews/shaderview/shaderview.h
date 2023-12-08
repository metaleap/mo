#pragma once

#include <ctime>
#include <cmath>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

#include "../appview.h"


struct ShaderView : AppView {
    ShaderView();

    void onUpdate(sf::Time delta);
    void onRender(sf::RenderWindow &window);

    bool setupAndLoadResources();

    std::time_t timeStarted = std::time(nullptr);
};
