#include <array>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

#include "./liveview.h"


struct {
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
} res;


LiveView::LiveView() {
    this->setupAndLoadResources();
}

LiveView::~LiveView() {
}

void LiveView::onUpdate(sf::Time) {
}

void LiveView::onRender(sf::RenderWindow &window) {
    window.draw(res.bgSprite);
}

void LiveView::setupAndLoadResources() {
    res.bgTexture.loadFromFile("/home/_/pix/mbuntu-2.jpg");
    res.bgSprite.setTexture(res.bgTexture);
    res.bgSprite.setPosition(0, 0);
}
