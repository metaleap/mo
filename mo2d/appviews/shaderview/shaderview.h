#pragma once

#include <filesystem>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>

#include "../appview.h"


struct Shader {
    std::filesystem::path filePath;
    std::filesystem::file_time_type fileModTime;
    std::string src = "";
    bool didLoadFail = false;
    bool isCurrent = false;
    bool didUserModifyLive = false;
};


struct ShaderView : AppView {
    ShaderView();

    void onUpdate(sf::Time delta);
    void onRender(sf::RenderWindow &window);

    bool setupAndLoadResources();
    void maybeReloadCurrentShader(Shader* curShader);

    std::time_t timeStarted = std::time(nullptr);
    std::time_t timeLastCheckedForChanges = 0;
    sf::Texture bgTexture;
    sf::RectangleShape rect;
    sf::Shader shader;
    std::vector<Shader> shaders;
};
