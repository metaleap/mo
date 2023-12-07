#include <array>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include "./liveview.h"
#include <SFML/System/Time.hpp>


struct {
    sf::Font overlayTextFont;
    sf::Text overlayTextTopLeft;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;
} res;


LiveView::LiveView() {
    this->setupAndLoadResources();
}


void LiveView::onUpdate(sf::Time delta) {
    res.overlayTextTopLeft.setString(sf::String("Sth: " + std::to_string(delta.asMilliseconds())));
}

void LiveView::onRender(sf::RenderWindow &window) {
    window.draw(res.bgSprite);
    window.draw(res.overlayTextTopLeft);
}

void LiveView::setupAndLoadResources() {
    res.overlayTextFont.loadFromFile("/home/_/heap/gd/kga-1.8.0/Other/Fonts/Kenney Future.ttf");
    res.overlayTextTopLeft.setFont(res.overlayTextFont);
    res.overlayTextTopLeft.setCharacterSize(77);
    res.overlayTextTopLeft.setFillColor(sf::Color::White);
    res.overlayTextTopLeft.setOutlineColor(sf::Color::Black);
    res.overlayTextTopLeft.setOutlineThickness(7.77f);
    const auto text_rect = res.overlayTextTopLeft.getLocalBounds();
    res.overlayTextTopLeft.setOrigin(text_rect.left + (text_rect.width * 0.5f), text_rect.top + (text_rect.height * 0.5f));
    res.overlayTextTopLeft.setPosition(44, 11);

    res.bgTexture.loadFromFile("/home/_/pix/mbuntu-2.jpg");
    res.bgSprite.setTexture(res.bgTexture);
    res.bgSprite.setPosition(0, 0);
}
