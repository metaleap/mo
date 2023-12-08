#include <SFML/Graphics/Shader.hpp>
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

#include "./appviews/liveview/liveview.h"
#include "./appviews/shaderview/shaderview.h"
#include "./gui/gui.h"
#include "appviews/appview.h"


struct {
    sf::Font fpsTextFont;
    sf::Text fpsTextTopLeft;
} res;


int main() {
    std::srand((int)(std::time(nullptr)));
    if (!sf::Shader::isAvailable())
        return 1;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "gfxdev");
    window.setPosition(sf::Vector2i(1920 / 2, 1080 / 2));
    // window.setVerticalSyncEnabled(true);

    void setupAndLoadResources();
    setupAndLoadResources();

    LiveView view_live;
    ShaderView view_shaders;

    std::vector<AppView> views;
    views.push_back(view_live);
    views.push_back(view_shaders);
    Gui gui(window, views);
    sf::Clock clock;
    clock.restart();

    while (window.isOpen()) {
        const auto delta = clock.restart();

        // INPUT
        sf::Event evt;
        while (window.pollEvent(evt)) {
            gui.onInput(evt);

            if ((evt.type == sf::Event::Closed) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                window.close();
                break;
            }
        }
        if (!window.isOpen())
            break;

        const auto delta_msec = delta.asMilliseconds();
        res.fpsTextTopLeft.setString(
            sf::String("fps: " + std::to_string((delta_msec == 0) ? (0) : (1000 / delta.asMilliseconds()))));
        view_live.onUpdate(delta);
        gui.onUpdate(delta);

        // RENDER
        window.clear();
        view_live.onRender(window);
        window.draw(res.fpsTextTopLeft);
        gui.onRender();
        window.display();
    }

    gui.onShutDown();
    return 0;
}

void setupAndLoadResources() {
    res.fpsTextFont.loadFromFile("/home/_/heap/gd/kga-1.8.0/Other/Fonts/Kenney Future.ttf");
    res.fpsTextTopLeft.setFont(res.fpsTextFont);
    res.fpsTextTopLeft.setCharacterSize(22);
    res.fpsTextTopLeft.setFillColor(sf::Color::White);
    res.fpsTextTopLeft.setOutlineColor(sf::Color::Black);
    res.fpsTextTopLeft.setOutlineThickness(3.21f);
    const auto text_rect = res.fpsTextTopLeft.getLocalBounds();
    res.fpsTextTopLeft.setOrigin(text_rect.left + (text_rect.width * 0.5f), text_rect.top + (text_rect.height * 0.5f));
    res.fpsTextTopLeft.setPosition(22, 11);
}
