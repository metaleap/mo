#pragma once

#include <cmath>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>


struct DynObj {
    bool active = false;
    float speedPxSec = 0.0;
};


struct Res;
struct Game {
    constexpr const static int8_t treeBranchesLimit = 6;
    constexpr const static float_t timeLimitSec = 6.0;

    float time_remaining_sec = timeLimitSec;
    bool paused = true;
    int score = 0;
    char initial = '!';
    bool isAlive = true;
    DynObj bee;
    DynObj cloud1;
    DynObj cloud2;
    DynObj cloud3;

  public:
    Game();
    void onUpdate(sf::Time delta);
    void onRender(sf::RenderWindow &window);

  private:
    void setupAndLoadResources();
    void updCloud(DynObj &cloud, sf::Sprite &sprite, const sf::Time &delta);
};
