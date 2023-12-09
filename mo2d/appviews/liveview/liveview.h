#pragma once

#include "../appview.h"


struct LiveView : AppView {
    LiveView();

    void onUpdate(sf::Time delta);
    void onRender(sf::RenderWindow &window);

    void setupAndLoadResources();
};
