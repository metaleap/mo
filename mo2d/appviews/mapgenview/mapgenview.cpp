#include <imgui.h>
#include <SFML/Graphics/Color.hpp>

#include "./libnoise_utils/noiseutils.h"

#include "./mapgenview.h"


MapGenView::MapGenView() {
    this->previewTinyRect.setSize({512, 256});
    this->previewFullRect.setSize({2048, 1024});
    this->previewTinyRect.setOrigin(0, 0);
    this->previewFullRect.setOrigin(0, 0);
    this->previewTinyRect.setPosition(768, 0);
    this->previewFullRect.setPosition(0, 320);
    this->previewTinyRect.setFillColor(sf::Color::Blue);
    this->previewFullRect.setFillColor(sf::Color::Blue);
}

void MapGenView::onUpdate(sf::Time) {
    ImGui::Begin("MapGen");
    if (ImGui::Button("Do it"))
        this->reGenerate();
    ImGui::End();
}

void MapGenView::onRender(sf::RenderWindow &window) {

    window.draw(this->previewTinyRect);
    window.draw(this->previewFullRect);
}

void MapGenView::reGenerate() {
}
