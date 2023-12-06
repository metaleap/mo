#include <array>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include "./game.h"


const float_t timeBarStartingWidth = 600;


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

    std::array<sf::Sprite, Game::treeBranchesLimit> treeBranches;
} res;


Game::Game() {
    this->setupAndLoadResources();
}


void Game::onUpdate(sf::Time delta) {
    time_remaining_sec -= delta.asSeconds();
    if (time_remaining_sec < 0)
        time_remaining_sec = timeLimitSec;

    updCloud(this->cloud1, res.cloud1Sprite, delta);
    updCloud(this->cloud2, res.cloud2Sprite, delta);
    updCloud(this->cloud3, res.cloud3Sprite, delta);
    if (!this->bee.active) {
        this->bee.active = true;
        this->bee.speedPxSec = 321 + std::rand() % 123;
        res.beeSprite.setPosition(2000, 444 + rand() % 222);
    } else {
        res.beeSprite.setPosition(res.beeSprite.getPosition().x - (this->bee.speedPxSec * delta.asSeconds()),
                                  res.beeSprite.getPosition().y);
        if (res.beeSprite.getPosition().x < -100)
            this->bee.active = false;
    }
    res.scoreText.setString(sf::String("Score: " + std::to_string(this->score)));
    sf::Time timeTotal;
    res.timeBar.setSize(sf::Vector2f((timeBarStartingWidth / timeLimitSec) * time_remaining_sec, 77));
}

void Game::onRender(sf::RenderWindow &window) {
    window.draw(res.bgSprite);
    window.draw(res.cloud1Sprite);
    window.draw(res.cloud2Sprite);
    window.draw(res.cloud3Sprite);
    window.draw(res.treeSprite);
    window.draw(res.beeSprite);
    window.draw(res.scoreText);
    window.draw(res.timeBar);
}

void Game::updCloud(DynObj &cloud, sf::Sprite &sprite, const sf::Time &delta) {
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

void Game::setupAndLoadResources() {
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
