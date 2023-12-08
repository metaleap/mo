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


bool setupAndLoadResources();
int main() {
    std::srand((int)(std::time(nullptr)));
    if (!sf::Shader::isAvailable())
        return 1;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "gfxdev");
    window.setPosition(sf::Vector2i(1920 / 2, 1080 / 2));
    // window.setVerticalSyncEnabled(true);

    if (!setupAndLoadResources())
        return 1;

    LiveView view_live;
    ShaderView view_shaders;

    AppView* view_current = &view_shaders;
    std::vector<AppView*> views = {&view_live, &view_shaders};
    Gui gui(window, views);

    sf::Clock clock;
    clock.restart();
    while (window.isOpen()) {
        const auto delta = clock.restart();

        // INPUT
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if ((evt.type == sf::Event::Closed) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                window.close();
                break;
            }
            for (auto key = sf::Keyboard::Num0; key <= sf::Keyboard::Num9; key = (sf::Keyboard::Key)(1 + (int)key)) {
                if (sf::Keyboard::isKeyPressed(key)) {
                    uint num = (int)key - (int)sf::Keyboard::Num0;
                    if ((num >= 1) && (num <= views.size()))
                        view_current = views[num - 1];
                }
            }

            gui.onInput(evt);
        }
        if (!window.isOpen())
            break;

        const auto delta_msec = delta.asMilliseconds();
        res.fpsTextTopLeft.setString(
            sf::String("fps: " + std::to_string((delta_msec == 0) ? (0) : (1000 / delta.asMilliseconds()))));
        gui.onUpdate(delta);
        view_current->onUpdate(delta);

        // RENDER
        window.clear();
        view_current->onRender(window);
        window.draw(res.fpsTextTopLeft);
        gui.onRender();
        window.display();
    }

    gui.onShutDown();
    fflush(stdout);
    return 0;
}

bool setupAndLoadResources() {
    if (!res.fpsTextFont.loadFromFile("/home/_/heap/gd/kga-1.8.0/Other/Fonts/Kenney Future.ttf"))
        return false;
    res.fpsTextTopLeft.setFont(res.fpsTextFont);
    res.fpsTextTopLeft.setCharacterSize(22);
    res.fpsTextTopLeft.setFillColor(sf::Color::White);
    res.fpsTextTopLeft.setOutlineColor(sf::Color::Black);
    res.fpsTextTopLeft.setOutlineThickness(3.21f);
    const auto text_rect = res.fpsTextTopLeft.getLocalBounds();
    res.fpsTextTopLeft.setOrigin(text_rect.left + (text_rect.width * 0.5f), text_rect.top + (text_rect.height * 0.5f));
    res.fpsTextTopLeft.setPosition(22, 11);
    return true;
}
