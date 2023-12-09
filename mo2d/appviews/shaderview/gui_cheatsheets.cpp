#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <string>
#include <vector>
#include <filesystem>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <imgui.h>

#include "shaderview.h"


const auto glslBuiltinNames = std::vector<std::string> {"none", "abs", "acos", "cos", "sin"};

std::string shaderSrcGlslBuiltinCheatsheetDefault = "";


void ShaderView::guiCheatSheets() {
    ImGui::Begin("Cheat Sheets");

    ImGui::End();
}

void ShaderView::ensureGlslBuiltinsCheatsheetImageFiles() {
    Shader tmp = Shader {.filePath = "../mo2d/appviews/shaderview/shaders/builtin_cheatsheet_preview_dont_rename.frag"};
    this->maybeReloadCurrentShader(&tmp, true, false);
    shaderSrcGlslBuiltinCheatsheetDefault = tmp.src;

    const uint size = 512;
    for (const auto &glsl_builtin_name : glslBuiltinNames) {
        const auto out_file_path = "../.local/glsl_builtins_pics/" + glsl_builtin_name + ".png";
        if (true || !std::filesystem::exists(out_file_path)) {
            sf::RenderTexture rt;
            if (!rt.create(size, size))
                abort();

            sf::Shader the_shader;
            if (!the_shader.loadFromMemory(std::string("#version 330\n#define fn ") + glsl_builtin_name + std::string("\n")
                                               + shaderSrcGlslBuiltinCheatsheetDefault,
                                           sf::Shader::Fragment))
                abort();

            sf::RectangleShape the_rect;
            the_rect.setOrigin(0, 0);
            the_rect.setPosition(0, 0);
            the_rect.setSize({(float)(size), (float)(size)});
            the_shader.setUniform("u_resolution", sf::Vector2f((float)(size), (float)(size)));
            rt.draw(the_rect, &the_shader);
            rt.display();

            const auto tex = rt.getTexture();
            const auto img = tex.copyToImage();
            if (!img.saveToFile(out_file_path))
                abort();
        }
    }
}
