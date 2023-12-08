#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

#include "./shaderview.h"
#include "imgui.h"


const auto shader_default = "uniform vec2 u_resolution;"
                            "uniform float u_time;"
                            "void main() {"
                            "	vec2 st = gl_FragCoord.xy/u_resolution;"
                            "	gl_FragColor = vec4(st.x,st.y,0.0,1.0);"
                            "}";


struct {
    sf::Texture bgTexture;
    sf::RectangleShape rect;
    sf::Shader shader;
    std::vector<std::filesystem::path> shaderFilePaths;
} res;


ShaderView::ShaderView() {
    if (!this->setupAndLoadResources())
        std::exit(1);

    for (const auto &entry : std::filesystem::directory_iterator("../mo2d/appviews/shaderview/shaders"))
        if (entry.path().extension() == ".frag")
            res.shaderFilePaths.push_back(entry.path());
}

void ShaderView::onUpdate(sf::Time) {
}

void ShaderView::onRender(sf::RenderWindow &window) {
    // const int64_t time_now = std::clock();
    // res.shader.setUniform("u_time", (float)time_now);
    const auto size_window = window.getSize();
    res.rect.setSize({(float)size_window.x, (float)size_window.y});
    res.shader.setUniform("u_resolution", sf::Vector2f((float)size_window.x, (float)size_window.y));
    window.draw(res.rect, &res.shader);

    ImGui::Begin("Shaders");
    for (const auto &shader_file_path : res.shaderFilePaths) {
        ImGui::Button(shader_file_path.filename().c_str());
    }
    ImGui::End();
}

bool ShaderView::setupAndLoadResources() {
    if (!res.bgTexture.loadFromFile("/home/_/pix/mbuntu-1.jpg"))
        return false;
    res.rect.setTexture(&res.bgTexture);

    return res.shader.loadFromMemory(shader_default, sf::Shader::Fragment);
}
