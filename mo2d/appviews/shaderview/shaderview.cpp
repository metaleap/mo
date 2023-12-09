#include <cstdio>
#include <filesystem>
#include <fstream>

#include "./shaderview.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>


const auto shader_default = "uniform vec2 u_resolution;"
                            "void main() {"
                            "	vec2 st = gl_FragCoord.xy/u_resolution;"
                            "	gl_FragColor = vec4(st.x,st.y,0.0,1.0);"
                            "}";
const time_t shader_hot_reload_interval_secs = 2;


ShaderView::ShaderView() {
    if (!this->setupAndLoadResources())
        std::exit(1);

    for (const auto &entry : std::filesystem::directory_iterator("../mo2d/appviews/shaderview/shaders"))
        if (entry.path().extension() == ".frag")
            this->shaders.push_back(Shader {.filePath = entry.path()});
}

void ShaderView::onUpdate(sf::Time) {
    ImGui::Begin("Shaders", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    Shader* shader_just_selected = nullptr;
    int shader_current_idx = -1;

    for (int i = 0; auto &it : this->shaders) {
        if (it.isCurrent) {
            shader_current_idx = i;
            break;
        }
        i++;
    }

    if (ImGui::BeginCombo("##Shaders",
                          (shader_current_idx < 0) ? "(Select a shader...)"
                                                   : this->shaders[shader_current_idx].filePath.filename().c_str(),
                          ImGuiComboFlags_HeightLargest)) {
        for (int i = 0; auto &it : this->shaders) {
            if (ImGui::Selectable(it.filePath.filename().c_str(), it.isCurrent)) {
                shader_current_idx = i;
                shader_just_selected = &it;
                break;
            }
            i++;
        }
        ImGui::EndCombo();
    }
    if (shader_just_selected != nullptr)
        for (auto &it : this->shaders)
            it.isCurrent = (&it == shader_just_selected);
    if (shader_current_idx >= 0) {
        const auto shader_current = &this->shaders[shader_current_idx];
        if (shader_just_selected != nullptr) {
            if (!shader_current->didUserModifyLive)
                this->maybeReloadCurrentShader(shader_current, true);
            else
                shader_current->didLoadFail = !this->shader.loadFromMemory(shader_current->src, sf::Shader::Fragment);
        } else if (ImGui::InputTextMultiline((std::string("##") + shader_current->filePath.filename().string()).c_str(),
                                             &shader_current->src)) {
            shader_current->didUserModifyLive = true;
            shader_current->didLoadFail = !this->shader.loadFromMemory(shader_current->src, sf::Shader::Fragment);
        } else if (!shader_current->didUserModifyLive) {
            const auto cur_sec = time(nullptr);
            if (((cur_sec % shader_hot_reload_interval_secs) == 0) && (cur_sec != shader_current->timeLastReloadedFromFs))
                maybeReloadCurrentShader(shader_current, false);
        }
        if (shader_current->didUserModifyLive && ImGui::Button("Reload from FS"))
            this->maybeReloadCurrentShader(shader_current, true);
    }
    ImGui::End();
}

void ShaderView::onRender(sf::RenderWindow &window) {
    // const int64_t time_now = std::clock();
    // res.shader.setUniform("u_time", (float)time_now);
    const auto size_window = window.getSize();
    this->rect.setSize({(float)size_window.x, (float)size_window.y});
    this->shader.setUniform("u_resolution", sf::Vector2f((float)size_window.x, (float)size_window.y));
    window.draw(this->rect, &this->shader);
}

void ShaderView::maybeReloadCurrentShader(Shader* curShader, bool force) {
    curShader->didUserModifyLive = false;
    std::ifstream file(curShader->filePath, std::ios_base::binary | std::ios_base::in);
    const auto src_new = std::string(std::istreambuf_iterator<char> {file}, std::istreambuf_iterator<char> {});
    if (force || (src_new != curShader->src)) {
        curShader->timeLastReloadedFromFs = time(nullptr);
        curShader->src = src_new;
        curShader->didLoadFail = !this->shader.loadFromMemory(curShader->src, sf::Shader::Fragment);
    }
}

bool ShaderView::setupAndLoadResources() {
    if (!this->bgTexture.loadFromFile("/home/_/pix/mbuntu-11.jpg"))
        return false;
    this->rect.setTexture(&this->bgTexture);

    return this->shader.loadFromMemory(shader_default, sf::Shader::Fragment);
}
