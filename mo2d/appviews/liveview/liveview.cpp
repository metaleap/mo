#include "./liveview.h"
#include <SFML/Window/Event.hpp>


LiveView::LiveView() {
    this->setupAndLoadResources();
}

void LiveView::onUpdate(const sf::Time &) {
}

void LiveView::onInput(const sf::Event &) {
}

void LiveView::onRender(sf::RenderWindow &window) {
    const auto size_tex = this->bgTexture.getSize();
    this->rect.setTextureRect({0, 0, (int)size_tex.x, (int)size_tex.y});
    const auto size_window = window.getView().getSize();
    this->rect.setSize({(float)size_window.x, (float)size_window.y});
    window.draw(this->rect);
}

void LiveView::setupAndLoadResources() {
    this->bgTexture.loadFromFile("/home/_/heap/gd/ease_cheatsheet.png");
    this->rect.setTexture(&this->bgTexture);
}
