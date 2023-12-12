#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>

#include <noise.h>

#include "./libnoise_utils/noiseutils.h"

#include "../appview.h"


struct MapGenView : AppView {
    MapGenView();

    void onUpdate(const sf::Time &delta);
    void onRender(sf::RenderWindow &window);
    void onInput(const sf::Event &evt);

    void reGenerate(bool tiny);
    void generateTileAndArea();

    std::string seedName = "foobar";
    noise::module::Perlin worldElevGen;
    utils::NoiseMap worldElevMap;
    float mouseX = -1.0;
    float mouseY = -1.0;
    float tileX = -1.0;
    float tileY = -1.0;
    sf::RectangleShape mapViewTinyRect;
    sf::Texture mapViewTinyTex;
    sf::RectangleShape mapViewFullRect;
    sf::Texture mapViewFullTex;
    sf::RectangleShape mapViewTileRect;
    sf::Texture mapViewTileTex;
    sf::RectangleShape mapViewAreaRect;
    sf::Texture mapViewAreaTex;
};
