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
    void prepTileableLayer(module::Module &module, utils::NoiseMap &destMap, double srcFreq, std::string outFilePath);

    std::string seedName = "foobar";
    noise::module::Perlin worldElevGen;
    utils::NoiseMap worldElevMap;
    utils::NoiseMap repRidged;
    utils::NoiseMap repBillow;
    sf::RectangleShape mapViewTinyRect;
    sf::RectangleShape tileSelRect;
    int mouseTileX = -1.0;
    int mouseTileY = -1.0;
    bool mouseTileLocked = false;
    int tileX = 240;
    int tileY = 265;
    sf::Texture mapViewTinyTex;
    sf::RectangleShape mapViewFullRect;
    sf::Texture mapViewFullTex;
    sf::RectangleShape mapViewTileRuffRect;
    sf::RectangleShape mapViewTileColRect;
    sf::Texture mapViewTileColTex;
    sf::RectangleShape mapViewTileBwRect;
    sf::Texture mapViewTileBwTex;
};
