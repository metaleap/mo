#include <array>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include "./shaderview.h"
#include <SFML/System/Time.hpp>


struct {
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
} res;


ShaderView::ShaderView() {
    this->setupAndLoadResources();
}


void ShaderView::onUpdate(sf::Time) {
}

void ShaderView::onRender(sf::RenderWindow &window) {
    window.draw(res.bgSprite);
}

void ShaderView::setupAndLoadResources() {
    res.bgTexture.loadFromFile("/home/_/pix/mbuntu-1.jpg");
    res.bgSprite.setTexture(res.bgTexture);
    res.bgSprite.setPosition(0, 0);
}
