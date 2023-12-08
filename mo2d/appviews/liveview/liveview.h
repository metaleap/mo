#pragma once

#include <cmath>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

#include "../appview.h"


struct LiveView : AppView {
    ~LiveView();
    LiveView();

    void onUpdate(sf::Time delta) override;
    void onRender(sf::RenderWindow &window) override;

    void setupAndLoadResources();
};
