#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
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
    void generateTile();
    void prepTileableLayer(module::Module &module, utils::NoiseMap &intoMap, std::string outFilePath);

    std::string seedName = "foo bar";
    noise::module::Perlin worldElevGen;
    utils::NoiseMap worldElevMap;
    utils::NoiseMap repRidged;
    utils::NoiseMap repBillow;
    sf::RectangleShape mapViewTinyRect;
    sf::RectangleShape tileSelRect;
    sf::Vector2i numTiles;
    float mouseX = -1.0;
    float mouseY = -1.0;
    int tileX = -1;
    int tileY = -1;
    sf::Texture mapViewTinyTex;
    sf::RectangleShape mapViewFullRect;
    sf::Texture mapViewFullTex;
    sf::RectangleShape mapViewTileRect;
    sf::Texture mapViewTileTex;
};
