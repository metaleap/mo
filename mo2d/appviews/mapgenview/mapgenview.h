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

    std::string seedName = "foo bar";
    noise::module::Perlin worldElev;

    sf::RectangleShape mapViewTinyRect;
    sf::Texture mapViewTinyTex;
    sf::RectangleShape mapViewFullRect;
    sf::Texture mapViewFullTex;
    sf::RectangleShape mapViewTileRect;
    sf::Texture mapViewTileTex;
    sf::RectangleShape mapViewAreaRect;
    sf::Texture mapViewAreaTex;
};
