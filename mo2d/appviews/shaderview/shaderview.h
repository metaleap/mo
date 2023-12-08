#pragma once

#include <cmath>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

#include "../appview.h"


struct ShaderView : AppView {
  public:
    ShaderView();
    void onUpdate(sf::Time delta);
    void onRender(sf::RenderWindow &window);

  private:
    void setupAndLoadResources();
};
