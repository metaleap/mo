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

#include "./liveview/liveview.h"
#include "./gui/gui.h"


int main() {
    std::srand((int)(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "gamedev");
    window.setPosition(sf::Vector2i(1920 / 2, 1080 / 2));
    window.setVerticalSyncEnabled(true);

    Gui gui(window);
    LiveView live_view;

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

        live_view.onUpdate(delta);
        gui.onUpdate(delta);

        // RENDER
        window.clear();
        live_view.onRender(window);
        gui.onRender();
        window.display();
    }

    gui.onShutDown();
    return 0;
}
