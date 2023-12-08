#include <array>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

#include "./liveview.h"


struct {
    sf::Texture bgTexture;
    sf::RectangleShape rect;
} res;


LiveView::LiveView() {
    this->setupAndLoadResources();
}

void LiveView::onUpdate(sf::Time) {
}

void LiveView::onRender(sf::RenderWindow &window) {
    const auto size_tex = res.bgTexture.getSize();
    res.rect.setTextureRect({0, 0, (int)size_tex.x, (int)size_tex.y});
    const auto size_window = window.getSize();
    res.rect.setSize({(float)size_window.x, (float)size_window.y});
    window.draw(res.rect);
}

void LiveView::setupAndLoadResources() {
    res.bgTexture.loadFromFile("/home/_/pix/mbuntu-0.jpg");
    res.rect.setTexture(&res.bgTexture);
}
