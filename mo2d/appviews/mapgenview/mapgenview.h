#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <noise.h>

#include "../appview.h"


struct MapGenView : AppView {
    MapGenView();

    void onUpdate(sf::Time delta);
    void onRender(sf::RenderWindow &window);

    void reGenerate(bool tiny);

    sf::RectangleShape previewTinyRect;
    sf::Texture previewTinyTex;
    sf::RectangleShape previewFullRect;
    sf::Texture previewFullTex;
    noise::module::Perlin perlinNoise;
};
