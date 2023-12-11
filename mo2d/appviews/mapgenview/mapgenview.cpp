#include "./mapgenview.h"


MapGenView::MapGenView() {
    this->previewTinyTex.loadFromFile("/home/_/heap/gd/ease_cheatsheet.png");
    this->previewTinyRect.setTexture(&this->previewTinyTex);
    const auto size_tex = this->previewTinyTex.getSize();
    this->previewTinyRect.setTextureRect({0, 0, (int)size_tex.x, (int)size_tex.y});
    this->previewTinyRect.setSize({2.0f * (float)(size_tex.x), 2.0f * (float)(size_tex.y)});
    this->previewTinyRect.setOrigin(0, 0);
    this->previewTinyRect.setPosition(0, 0);
}

void MapGenView::onUpdate(sf::Time) {
}

void MapGenView::onRender(sf::RenderWindow &window) {
    window.draw(this->previewTinyRect);
}
