#include <SFML/Graphics/Image.hpp>
#include <cstdio>
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


const auto glslBuiltinNames1D = std::vector<std::string> {
    "none",  "abs",       "acos",  "acosh", "asin",   "asinh",       "atan",   "atanh", "ceil", "cos",    "cosh",
    "exp",   "exp2",      "floor", "fract", "fwidth", "inversesqrt", "length", "log",   "log2", "noise1", "normalize",
    "round", "roundEven", "sign",  "sin",   "sinh",   "sqrt",        "tan",    "tanh",  "trunc"};
const auto glslBuiltinNames2D = std::vector<std::string> {"atan", "max", "min", "mod", "pow", "reflect"};

std::map<std::string, sf::Texture> glslBuiltinTextures {};
std::string shaderSrcGlslBuiltin1D = "";
std::string shaderSrcGlslBuiltin2D = "";


void ShaderView::guiCheatSheets() {
    if (ImGui::Begin("Cheat Sheets")) {
        if (ImGui::TreeNode("GLSL funcs")) {
            bool did_expand_all = ImGui::Button("Expand all");
            bool did_collapse_all = ImGui::Button("Collapse all");
            for (const auto &pair : std::map<std::string, std::vector<std::string>> {
                     {"1D", glslBuiltinNames1D},
                     {"2D", glslBuiltinNames2D},
                 })
                for (const auto &glsl_builtin_name : pair.second) {
                    if (did_expand_all)
                        ImGui::SetNextItemOpen(true);
                    else if (did_collapse_all)
                        ImGui::SetNextItemOpen(false);
                    if (ImGui::TreeNode((glsl_builtin_name + " (" + pair.first + ")").c_str())) {
                        const auto &tex = glslBuiltinTextures[glsl_builtin_name + "_" + pair.first];
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
    {
        Shader tmp;
        tmp.filePath = "../mo2d/appviews/shaderview/shaders/glsl_builtin_1d_cheatsheet_preview_dont_rename.frag";
        this->maybeReloadCurrentShader(&tmp, true, false);
        shaderSrcGlslBuiltin1D = tmp.src;
        tmp.filePath = "../mo2d/appviews/shaderview/shaders/glsl_builtin_2d_cheatsheet_preview_dont_rename.frag";
        this->maybeReloadCurrentShader(&tmp, true, false);
        shaderSrcGlslBuiltin2D = tmp.src;
    }

    const uint size = 512;
    bool is2d = false;
    for (const auto &pair : std::map<std::string, std::vector<std::string>> {
             {"1D", glslBuiltinNames1D},
             {"2D", glslBuiltinNames2D},
         }) {
        for (const auto &glsl_builtin_name : pair.second) {
            const auto out_file_path = "../.local/glsl_builtins_pics/" + glsl_builtin_name + "_" + pair.first + ".png";
            if (is2d || !std::filesystem::exists(out_file_path)) {
                printf("%s\n", out_file_path.c_str());
                fflush(stdout);
                sf::RenderTexture rt;
                if (!rt.create(size, size))
                    abort();

                sf::Shader the_shader;
                if (!the_shader.loadFromMemory(std::string("#version 450\n#define fn ") + glsl_builtin_name
                                                   + std::string("\n")
                                                   + (is2d ? shaderSrcGlslBuiltin2D : shaderSrcGlslBuiltin1D),
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
            glslBuiltinTextures[glsl_builtin_name + "_" + pair.first] = tex;
        }
        is2d = true;
    }
}
