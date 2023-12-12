#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <map>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "./libnoise_utils/noiseutils.h"

#include "./mapgenview.h"



float mix(float x, float y, float a) {
    return (x * (1.0f - a)) + (y * a);
}


MapGenView::MapGenView() {
    this->perlinNoise.SetLacunarity(2.121);
    this->perlinNoise.SetOctaveCount(30);
    this->perlinNoise.SetSeed(12);
    this->perlinNoise.SetPersistence(0.515);
    this->perlinNoise.SetFrequency(1.221);

    this->previewTinyRect.setSize({1024, 512});
    this->previewFullRect.setSize({3072, 1536});
    this->previewTinyRect.setOrigin(0, 0);
    this->previewFullRect.setOrigin(0, 0);
    this->previewTinyRect.setPosition(768, 0);
    this->previewFullRect.setPosition(0, 624);
    this->previewTinyRect.setFillColor(sf::Color::White);
    this->previewFullRect.setFillColor(sf::Color::White);

    this->previewFullTex.setSmooth(true);
    this->previewFullTex.setRepeated(false);
    this->previewTinyTex.setSmooth(true);
    this->previewTinyTex.setRepeated(false);
}

void MapGenView::onUpdate(sf::Time) {
    ImGui::Begin("MapGen");
    {
        if (ImGui::InputText("Map Name", &this->seedName)) {
            std::transform(this->seedName.begin(), this->seedName.end(), this->seedName.begin(), tolower);
            auto upper = std::string(this->seedName);
            std::transform(upper.begin(), upper.end(), upper.begin(), toupper);
            std::hash<std::string> hasher;
            auto hash = hasher(this->seedName + upper);
            this->perlinNoise.SetSeed((int)hash);
        }
        float gappiness = (float)(perlinNoise.GetLacunarity());
        if (ImGui::InputFloat("Lacunarity (Gappiness)", &gappiness))
            perlinNoise.SetLacunarity(gappiness);
        float roughness = (float)(perlinNoise.GetPersistence());
        if (ImGui::InputFloat("Roughness (Persistence)", &roughness))
            perlinNoise.SetPersistence(roughness);
        float extremeness = (float)(perlinNoise.GetFrequency());
        if (ImGui::InputFloat("Extreme-ness (Frequency)", &extremeness))
            perlinNoise.SetFrequency(extremeness);
    }
    if (ImGui::Button("Tiny"))
        this->reGenerate(true);
    if (ImGui::Button("Full"))
        this->reGenerate(false);
    ImGui::End();
}

void MapGenView::onRender(sf::RenderWindow &window) {
    window.draw(this->previewTinyRect);
    window.draw(this->previewFullRect);
}

void MapGenView::reGenerate(bool tiny) {
    clock_t t_start = clock();
    perlinNoise.SetNoiseQuality(NoiseQuality::QUALITY_BEST);

    module::ScaleBias scaler;
    scaler.SetSourceModule(0, perlinNoise);
    scaler.SetScale(0.7);

    {
        utils::NoiseMap heightMap;
        utils::NoiseMapBuilderSphere heightMapBuilder;
        heightMapBuilder.SetSourceModule(scaler);
        heightMapBuilder.SetDestNoiseMap(heightMap);
        heightMapBuilder.SetDestSize(tiny ? 512 : 4096, tiny ? 256 : 2048);
        heightMapBuilder.SetBounds(-90.0, 90.0, -180.0, 180.0);
        heightMapBuilder.Build();
        clock_t t_end = clock();
        for (int y = 0, h = heightMap.GetHeight(), y_h = h / 8; y < y_h; y++) {
            const float weight = sqrt(sqrt(((float)y / (float)y_h))); // (0 -> force ocean) .. (1 -> keep as-is)
            for (int x = 0, w = heightMap.GetWidth(); x < w; x++) {
                auto value = heightMap.GetValue(x, y);
                heightMap.SetValue(x, y, mix(-1, value, weight));
                value = heightMap.GetValue(x, h - y);
                heightMap.SetValue(x, h - y, mix(-1, value, weight));
            }
        }
        if (!tiny) {
            for (int w = heightMap.GetWidth(), x = 0, h = heightMap.GetHeight(); x < w; x++)
                for (int y = 0; y < h; y++)
                    if (((y % (h / 8)) == 0) && (((x / 128) % 2) == 0))
                        heightMap.SetValue(x, y, 1.234f);
            printf("heights done: %fsec\n", (float)(t_end - t_start) / CLOCKS_PER_SEC);
        }

        utils::RendererImage renderer;
        renderer.SetSourceNoiseMap(heightMap);
        { // coloring
            renderer.ClearGradient();
            renderer.AddGradientPoint(-1.111, utils::Color(0, 0, 128, 255));   // very-deeps
            renderer.AddGradientPoint(-0.220, utils::Color(0, 0, 255, 255));   // water
            renderer.AddGradientPoint(-0.011, utils::Color(0, 128, 255, 255)); // shoal
            renderer.AddGradientPoint(-0.001, utils::Color(0, 128, 255, 255)); // shoal
            renderer.AddGradientPoint(0.000, utils::Color(128, 128, 32, 255)); // sand
            renderer.AddGradientPoint(0.002, utils::Color(128, 160, 0, 255));  // grass
            renderer.AddGradientPoint(0.125, utils::Color(32, 160, 0, 255));   // grass
            renderer.AddGradientPoint(0.375, utils::Color(128, 128, 128, 255));
            renderer.AddGradientPoint(0.750, utils::Color(196, 196, 196, 255));
            renderer.AddGradientPoint(1.111, utils::Color(244, 244, 244, 255));
            renderer.AddGradientPoint(1.234, utils::Color(255, 128, 0, 255));
            renderer.EnableLight();
            renderer.SetLightContrast(3.21);
            renderer.SetLightBrightness(2.34);
        }
        utils::Image image;
        renderer.SetDestImage(image);
        t_start = clock();
        renderer.Render();

        const auto out_file_path =
            std::filesystem::absolute("../.local/temp_" + std::string(tiny ? "tiny" : "full") + ".bmp");
        utils::WriterBMP writer;
        writer.SetSourceImage(image);
        writer.SetDestFilename(out_file_path);
        writer.WriteDestFile();
        t_end = clock();

        if (tiny) {
            this->previewTinyTex.loadFromFile(out_file_path);
            this->previewTinyRect.setTexture(&this->previewTinyTex);
            const auto size_tex = this->previewTinyTex.getSize();
            this->previewTinyRect.setTextureRect(sf::IntRect {0, 0, (int)(size_tex.x), (int)(size_tex.y)});
        } else {
            this->previewFullTex.loadFromFile(out_file_path);
            this->previewFullRect.setTexture(&this->previewFullTex);
            const auto size_tex = this->previewFullTex.getSize();
            this->previewFullRect.setTextureRect(sf::IntRect {0, 0, (int)(size_tex.x), (int)(size_tex.y)});
            this->reGenerate(true);
        }

        printf("%s: %fsec\n", out_file_path.c_str(), (float)(t_end - t_start) / CLOCKS_PER_SEC);
        fflush(stdout);
    }
}
