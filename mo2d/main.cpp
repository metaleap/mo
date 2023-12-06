#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <ostream>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include "gui/gui.h"

const float timeBarStartingWidth = 600;
const float timeLimitSec = 6;
const int treeBranchesLimit = 6;

typedef struct {
    bool active;
    float speedPxSec;
} DynObj;

struct {
    bool paused;
    int score;
    char initial;
    bool isAlive;
    DynObj bee;
    DynObj cloud1;
    DynObj cloud2;
    DynObj cloud3;
} game = {.paused = true,
          .score = 0,
          .initial = 'Z',
          .isAlive = true,
          .bee = {.active = false, .speedPxSec = 0.0},
          .cloud1 = {.active = false, .speedPxSec = 0.0},
          .cloud2 = {.active = false, .speedPxSec = 0.0},
          .cloud3 = {.active = false, .speedPxSec = 0.0}};

struct {
    sf::Font scoreFont;
    sf::Text scoreText;

    sf::RectangleShape timeBar;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    sf::Texture treeTexture;
    sf::Sprite treeSprite;

    sf::Texture beeTexture;
    sf::Sprite beeSprite;

    sf::Texture cloudTexture;
    sf::Sprite cloud1Sprite;
    sf::Sprite cloud2Sprite;
    sf::Sprite cloud3Sprite;

    std::array<sf::Sprite, treeBranchesLimit> treeBranches;
} res;

void setupAndLoadResources();
void updCloud(DynObj &, sf::Sprite &, const sf::Time &);
void updTreeBranches();

int main() {
    std::printf("You got _that_ right. This here is the one 'Console' this'll "
                "run on.\n...or will it?...\n");
    std::fflush(stdout);

    std::srand((int)(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "gamedev");
    window.setPosition(sf::Vector2i(1920 / 2, 1080 / 2));
    window.setVerticalSyncEnabled(true);

    Gui gui(window);

    sf::Clock clock;
    clock.restart();
    setupAndLoadResources();
    auto time_remaining_sec = timeLimitSec;

    while (window.isOpen()) {
        const auto delta = clock.restart();
        time_remaining_sec = time_remaining_sec - delta.asSeconds();
        if (time_remaining_sec < 0)
            time_remaining_sec = timeLimitSec;

        // INPUT
        sf::Event evt;
        while (window.pollEvent(evt)) {
            gui.processEvent(evt);

            if ((evt.type == sf::Event::Closed) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                window.close();
                gui.onShutDown();
                return 0;
            } else if ((evt.type == sf::Event::KeyPressed) && (evt.key.code == sf::Keyboard::Enter)) {
                time_remaining_sec = timeLimitSec;
                game.paused = !game.paused;
            }
        }
        if (!window.hasFocus())
            game.paused = true;
        if (game.paused) {
            window.clear();
            window.display();
            sf::sleep(sf::milliseconds(123));
            continue;
        }

        // UPDATES
        updCloud(game.cloud1, res.cloud1Sprite, delta);
        updCloud(game.cloud2, res.cloud2Sprite, delta);
        updCloud(game.cloud3, res.cloud3Sprite, delta);
        if (!game.bee.active) {
            game.bee.active = true;
            game.bee.speedPxSec = 321 + std::rand() % 123;
            res.beeSprite.setPosition(2000, 444 + rand() % 222);
        } else {
            res.beeSprite.setPosition(res.beeSprite.getPosition().x - (game.bee.speedPxSec * delta.asSeconds()),
                                      res.beeSprite.getPosition().y);
            if (res.beeSprite.getPosition().x < -100)
                game.bee.active = false;
        }
        res.scoreText.setString(sf::String("Score: " + std::to_string(game.score)));
        sf::Time timeTotal;
        res.timeBar.setSize(sf::Vector2f((timeBarStartingWidth / timeLimitSec) * time_remaining_sec, 77));

        gui.update(delta);

        // RENDER
        window.clear();
        window.draw(res.bgSprite);
        window.draw(res.cloud1Sprite);
        window.draw(res.cloud2Sprite);
        window.draw(res.cloud3Sprite);
        window.draw(res.treeSprite);
        window.draw(res.beeSprite);
        window.draw(res.scoreText);
        window.draw(res.timeBar);
        gui.render();
        window.display();
    }

    gui.onShutDown();
    return 0;
}

void updCloud(DynObj &cloud, sf::Sprite &sprite, const sf::Time &delta) {
    if (!cloud.active) {
        cloud.speedPxSec = 22 + (std::rand() % 22);
        sprite.setPosition(-321, (rand() % 123));
        cloud.active = true;
    } else {
        sprite.setPosition(sprite.getPosition().x + (cloud.speedPxSec * delta.asSeconds()), sprite.getPosition().y);
        if (sprite.getPosition().x > 1920)
            cloud.active = false;
    }
}

void setupAndLoadResources() {
    res.timeBar.setSize(sf::Vector2f(timeBarStartingWidth, 77));
    res.timeBar.setFillColor(sf::Color::Red);
    res.timeBar.setPosition(sf::Vector2f((1920.0f * 0.5) - (timeBarStartingWidth * 0.5f), 980));

    res.scoreFont.loadFromFile("/home/_/heap/gd/kga-1.8.0/Other/Fonts/Kenney Future.ttf");
    res.scoreText.setFont(res.scoreFont);
    res.scoreText.setCharacterSize(77);
    res.scoreText.setFillColor(sf::Color::White);
    res.scoreText.setOutlineColor(sf::Color::Black);
    res.scoreText.setOutlineThickness(7.77f);
    const auto text_rect = res.scoreText.getLocalBounds();
    res.scoreText.setOrigin(text_rect.left + (text_rect.width * 0.5f), text_rect.top + (text_rect.height * 0.5f));
    res.scoreText.setPosition(44, 11);

    res.bgTexture.loadFromFile("/home/_/pix/mbuntu-6.jpg");
    res.bgSprite.setTexture(res.bgTexture);
    res.bgSprite.setPosition(0, 0);

    res.treeTexture.loadFromFile("/home/_/heap/gd/kga-1.8.0/3D assets/Retro Urban Kit/Models/FBX "
                                 "format/Textures/treeB.png");
    res.treeSprite.setTexture(res.treeTexture);
    res.treeSprite.setScale(sf::Vector2f(11, 22));
    res.treeSprite.setPosition(810, -222);

    res.beeTexture.loadFromFile("/home/_/heap/gd/kga-1.8.0/2D assets/Platformer "
                                "Pack Redux/PNG/Enemies/bee.png");
    res.beeSprite.setTexture(res.beeTexture);
    res.beeSprite.setPosition(111, 222);

    res.cloudTexture.loadFromFile("/home/_/heap/gd/kga-1.8.0/2D assets/Jumper Pack/PNG/Enemies/cloud.png");
    res.cloud1Sprite.setTexture(res.cloudTexture);
    res.cloud1Sprite.setPosition(222, 77);
    res.cloud2Sprite.setTexture(res.cloudTexture);
    res.cloud2Sprite.setPosition(777, 22);
    res.cloud3Sprite.setTexture(res.cloudTexture);
    res.cloud3Sprite.setPosition(1661, 234);
}
