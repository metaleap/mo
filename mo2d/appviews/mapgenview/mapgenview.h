#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <SFML/Window/Event.hpp>
#include <noise.h>

#include "../appview.h"


struct MapGenView : AppView {
    MapGenView();

    void onUpdate(const sf::Time &delta);
    void onRender(sf::RenderWindow &window);
    void onInput(const sf::Event &evt);

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
