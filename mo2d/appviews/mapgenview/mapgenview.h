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
    void generateTileAndArea();

    std::string seedName = "foo bar";
    noise::module::Perlin worldElev;
    float mouseLon = 0.0;
    float mouseLat = 0.0;
    float tileLon = 0.0;
    float tileLat = 0.0;
    sf::RectangleShape mapViewTinyRect;
    sf::Texture mapViewTinyTex;
    sf::RectangleShape mapViewFullRect;
    sf::Texture mapViewFullTex;
    sf::RectangleShape mapViewTileRect;
    sf::Texture mapViewTileTex;
    sf::RectangleShape mapViewAreaRect;
    sf::Texture mapViewAreaTex;
};
