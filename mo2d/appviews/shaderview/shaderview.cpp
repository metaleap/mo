#include <cstdio>
#include <fstream>

#include "./shaderview.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>


const auto shader_default = "uniform vec2 u_resolution;"
                            "uniform float u_time;"
                            "void main() {"
                            "	vec2 st = gl_FragCoord.xy/u_resolution;"
                            "	gl_FragColor = vec4(st.x,st.y,0.0,1.0);"
                            "}";


ShaderView::ShaderView() {
    if (!this->setupAndLoadResources())
        std::exit(1);

    for (const auto &entry : std::filesystem::directory_iterator("../mo2d/appviews/shaderview/shaders"))
        if (entry.path().extension() == ".frag")
            this->shaders.push_back(Shader {.filePath = entry.path(), .fileModTime = entry.last_write_time()});
}

void ShaderView::onUpdate(sf::Time) {
    ImGui::Begin("Shaders", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    Shader* shader_just_selected = nullptr;
    int shader_current_idx = -1;

    for (int i = 0; auto &it : this->shaders) {
        if (it.isCurrent)
            shader_current_idx = i;
        i++;
    }

    if (ImGui::BeginCombo("##Shaders",
                          (shader_current_idx < 0) ? "(Select a shader...)"
                                                   : this->shaders[shader_current_idx].filePath.filename().c_str(),
                          ImGuiComboFlags_HeightLargest)) {
        for (auto &it : this->shaders)
            if (ImGui::Selectable(it.filePath.filename().c_str(), it.isCurrent))
                shader_just_selected = &it;
        ImGui::EndCombo();
    }
    for (int i = 0; auto &it : this->shaders) {
        if (shader_just_selected != nullptr)
            it.isCurrent = (&it == shader_just_selected);
        if (it.isCurrent)
            shader_current_idx = i;
        i++;
    }
    if (shader_current_idx >= 0) {
        const auto shader_current = &this->shaders[shader_current_idx];
        if (shader_just_selected != nullptr)
            this->maybeReloadCurrentShader(shader_current);
        else {
            if (ImGui::InputTextMultiline((std::string("##") + shader_current->filePath.filename().string()).c_str(),
                                          &shader_current->src)) {
                shader_current->didUserModifyLive = true;
                shader_current->didLoadFail = !this->shader.loadFromMemory(shader_current->src, sf::Shader::Fragment);
            } else if (!shader_current->didUserModifyLive) {
                const auto cur_sec = time(nullptr);
                if (((cur_sec % 3) == 0) && (cur_sec != this->timeLastCheckedForChanges))
                    maybeReloadCurrentShader(shader_current);
            }
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
    this->rect.setSize({(float)size_window.x, (float)size_window.y});
    this->shader.setUniform("u_resolution", sf::Vector2f((float)size_window.x, (float)size_window.y));
    window.draw(this->rect, &this->shader);
}

void ShaderView::maybeReloadCurrentShader(Shader* curShader) {
    std::ifstream file(curShader->filePath, std::ios_base::binary | std::ios_base::in);
    const auto src_new = std::string(std::istreambuf_iterator<char> {file}, std::istreambuf_iterator<char> {});
    if (src_new != curShader->src) {
        curShader->src = src_new;
        curShader->didLoadFail = !this->shader.loadFromMemory(curShader->src, sf::Shader::Fragment);
        curShader->didUserModifyLive = false;
        this->timeLastCheckedForChanges = time(nullptr);
    }
}

bool ShaderView::setupAndLoadResources() {
    if (!this->bgTexture.loadFromFile("/home/_/pix/mbuntu-11.jpg"))
        return false;
    this->rect.setTexture(&this->bgTexture);

    return this->shader.loadFromMemory(shader_default, sf::Shader::Fragment);
}
