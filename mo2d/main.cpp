#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "./gui/gui.h"
#include "./appviews/liveview/liveview.h"
#include "./appviews/shaderview/shaderview.h"
#include "./appviews/mapgenview/mapgenview.h"


struct {
    sf::Font fpsTextFont;
    sf::Text fpsTextTopLeft;
} res;


bool setupAndLoadResources();
int main() {
    std::srand((int)(std::time(nullptr)));
    if (!sf::Shader::isAvailable())
        return 1;

    sf::RenderWindow window(sf::VideoMode(2560, 1440), "gfxdev");
    window.setPosition(sf::Vector2i(2560 / 4, 1440 / 4));
    window.setVerticalSyncEnabled(true);

    if (!setupAndLoadResources())
        return 1;

    LiveView view_live;
    ShaderView view_shaders;
    MapGenView view_mapgen;

    AppView* view_current = &view_shaders;
    std::vector<AppView*> views = {&view_live, &view_shaders, &view_mapgen};
    Gui gui(window, views);

    sf::Clock clock;
    clock.restart();
    while (window.isOpen()) {
        const auto delta = clock.restart();

        // INPUT
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Resized)
                window.setView(sf::View(sf::FloatRect(0, 0, (float)(evt.size.width), (float)(evt.size.height))));
            if ((evt.type == sf::Event::Closed)
                || (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))) {
                window.close();
                break;
            }
            for (auto key = sf::Keyboard::F1; key <= sf::Keyboard::F12; key = (sf::Keyboard::Key)(1 + (int)key)) {
                if (sf::Keyboard::isKeyPressed(key)) {
                    uint idx = (int)key - (int)sf::Keyboard::F1;
                    if ((idx >= 0) && (idx < views.size()))
                        view_current = views[idx];
                    break;
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
