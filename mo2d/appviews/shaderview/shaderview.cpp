#include <fstream>

#include "./shaderview.h"


ShaderView::ShaderView() {
    if (!this->setupAndLoadResources())
        std::exit(1);

    this->shaders.push_back(Shader {.src = shaderSrcScratchpadDefault, .isCurrent = true});
    for (const auto &entry : std::filesystem::directory_iterator("../mo2d/appviews/shaderview/shaders"))
        if (entry.path().extension() == ".frag")
            this->shaders.push_back(Shader {.filePath = entry.path()});
}

void ShaderView::onUpdate(sf::Time) {
    this->guiShaders();
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
    if (!this->bgTexture.loadFromFile("/home/_/heap/gd/kynds_curves.png"))
        return false;
    this->rect.setTexture(&this->bgTexture);

    return this->shader.loadFromMemory(shaderSrcScratchpadDefault, sf::Shader::Fragment);
}
