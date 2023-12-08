#include <array>
#include <cstddef>
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
#include <string>

#include "./shaderview.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>


const auto shader_default = "uniform vec2 u_resolution;"
                            "uniform float u_time;"
                            "void main() {"
                            "	vec2 st = gl_FragCoord.xy/u_resolution;"
                            "	gl_FragColor = vec4(st.x,st.y,0.0,1.0);"
                            "}";

struct Shader {
    std::filesystem::path filePath;
    std::filesystem::file_time_type fileModTime;
    std::string src = "";
    bool isCurrent = false;
};

struct {
    sf::Texture bgTexture;
    sf::RectangleShape rect;
    sf::Shader shader;
    std::vector<Shader> shaders;
} res;


ShaderView::ShaderView() {
    if (!this->setupAndLoadResources())
        std::exit(1);

    for (const auto &entry : std::filesystem::directory_iterator("../mo2d/appviews/shaderview/shaders"))
        if (entry.path().extension() == ".frag")
            res.shaders.push_back(Shader {.filePath = entry.path(), .fileModTime = entry.last_write_time()});
}

void ShaderView::onUpdate(sf::Time delta) {
    ImGui::Begin("Shaders");
    Shader* shader_just_selected = nullptr;
    int shader_current_idx = -1;

    if (ImGui::BeginCombo("Hola", "", ImGuiComboFlags_NoPreview | ImGuiComboFlags_HeightLargest)) {
        for (auto &shader : res.shaders)
            if (ImGui::Selectable(shader.filePath.filename().c_str(), shader.isCurrent))
                shader_just_selected = &shader;
        ImGui::EndCombo();
    }
    for (int i = 0; auto &shader : res.shaders) {
        if (shader_just_selected != nullptr)
            shader.isCurrent = (&shader == shader_just_selected);
        if (shader.isCurrent)
            shader_current_idx = i;
        i++;
    }
    if (shader_current_idx >= 0) {
        const auto shader_current = &res.shaders[shader_current_idx];
        if (ImGui::InputTextMultiline(shader_current->filePath.filename().c_str(), &shader_current->src)) {
            if (!res.shader.loadFromMemory(shader_current->src, sf::Shader::Fragment))
                ImGui::Text("Shader load error, check stdout/stderr");
        }
    }
    // if (shader.isCurrent) {
    // }
    ImGui::End();
}

void ShaderView::onRender(sf::RenderWindow &window) {
    // const int64_t time_now = std::clock();
    // res.shader.setUniform("u_time", (float)time_now);
    const auto size_window = window.getSize();
    res.rect.setSize({(float)size_window.x, (float)size_window.y});
    res.shader.setUniform("u_resolution", sf::Vector2f((float)size_window.x, (float)size_window.y));
    window.draw(res.rect, &res.shader);
}

bool ShaderView::setupAndLoadResources() {
    if (!res.bgTexture.loadFromFile("/home/_/pix/mbuntu-1.jpg"))
        return false;
    res.rect.setTexture(&res.bgTexture);

    return res.shader.loadFromMemory(shader_default, sf::Shader::Fragment);
}
