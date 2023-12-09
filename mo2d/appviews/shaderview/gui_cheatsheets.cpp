#include <SFML/Graphics/Image.hpp>
#include <cstdlib>
#include <string>
#include <vector>
#include <filesystem>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <imgui.h>

#include "shaderview.h"


const auto glslBuiltinNames = std::vector<std::string> {"none", "abs", "acos", "cos", "sin"};

std::map<std::string, sf::Texture> glslBuiltinTextures {};
std::string shaderSrcGlslBuiltinCheatsheetDefault = "";


void ShaderView::guiCheatSheets() {
    if (ImGui::Begin("Cheat Sheets")) {
        if (ImGui::TreeNode("GLSL funcs")) {
            bool did_expand_all = ImGui::Button("Expand all");
            bool did_collapse_all = ImGui::Button("Collapse all");
            for (const auto &glsl_builtin_name : glslBuiltinNames) {
                if (did_expand_all)
                    ImGui::SetNextItemOpen(true);
                else if (did_collapse_all)
                    ImGui::SetNextItemOpen(false);
                if (ImGui::TreeNode(glsl_builtin_name.c_str())) {
                    const auto &tex = glslBuiltinTextures[glsl_builtin_name];
                    ImGui::Image((void*)(uint64_t)(tex.getNativeHandle()), {512, 512});
                    ImGui::TreePop();
                } else
                    did_expand_all = false;
            }
            ImGui::TreePop();
        }
    };
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
        sf::Texture tex;
        if (!tex.loadFromFile(out_file_path))
            abort();
        glslBuiltinTextures[glsl_builtin_name] = tex;
    }
}
