#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <map>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "./mapgenview.h"
#include "libnoise_utils/noiseutils.h"



float mix(float x, float y, float a) {
    return (x * (1.0f - a)) + (y * a);
}


MapGenView::MapGenView() {
    this->worldElevGen.SetNoiseQuality(NoiseQuality::QUALITY_BEST);
    this->worldElevGen.SetLacunarity(2.121);
    this->worldElevGen.SetOctaveCount(30);
    this->worldElevGen.SetSeed(12);
    this->worldElevGen.SetPersistence(0.515);
    this->worldElevGen.SetFrequency(1.221);

    for (auto rect_and_tex : std::map<sf::Texture*, sf::RectangleShape*> {
             {&this->mapViewTinyTex, &this->mapViewTinyRect},
             {&this->mapViewFullTex, &this->mapViewFullRect},
             {&this->mapViewTileTex, &this->mapViewTileRect},
             {&this->mapViewAreaTex, &this->mapViewAreaRect},
         }) {
        rect_and_tex.second->setOrigin(0, 0);
        rect_and_tex.second->setFillColor(sf::Color::White);
        rect_and_tex.first->setSmooth(true);
        rect_and_tex.first->setRepeated(false);
    }

    this->mapViewFullRect.setPosition(0, 624);
    this->mapViewFullRect.setSize({3072, 1536});

    this->mapViewTinyRect.setPosition(0, 0);
    this->mapViewTinyRect.setSize({1024, 512});

    this->mapViewAreaRect.setPosition(1024 + 128, 0);
    this->mapViewAreaRect.setSize({512, 512});

    this->mapViewTileRect.setPosition(1024 + 128 + 512 + 128, 0);
    this->mapViewTileRect.setSize({512, 512});
}

void MapGenView::onInput(const sf::Event &evt) {
    if (evt.type == sf::Event::MouseMoved) {
        auto mx = (float)evt.mouseMove.x, my = (float)evt.mouseMove.y;
        const auto pos_mapview = mapViewFullRect.getPosition();
        const auto size_mapview = mapViewFullRect.getSize();
        if ((mx >= pos_mapview.x) && (mx <= (pos_mapview.x + size_mapview.x)) && (my >= pos_mapview.y)
            && (my <= (pos_mapview.y + size_mapview.y))) {
            this->mouseX = (mx - pos_mapview.x) / size_mapview.x; // 0..1
            this->mouseY = (my - pos_mapview.y) / size_mapview.y; // 0..1
        }
    }
    if (evt.type == sf::Event::MouseButtonReleased) {
        this->tileX = this->mouseX;
        this->tileY = this->mouseY;
        this->generateTileAndArea();
    }
}

void MapGenView::onUpdate(const sf::Time &) {
    ImGui::Begin("MapGen");
    {
        if (ImGui::InputText("Map Name", &this->seedName)) {
            std::transform(this->seedName.begin(), this->seedName.end(), this->seedName.begin(), tolower);
            auto upper = std::string(this->seedName);
            std::transform(upper.begin(), upper.end(), upper.begin(), toupper);
            std::hash<std::string> hasher;
            auto hash = hasher(this->seedName + upper);
            this->worldElevGen.SetSeed((int)hash);
        }
        float gappiness = (float)(worldElevGen.GetLacunarity());
        if (ImGui::InputFloat("Lacunarity (Gappiness)", &gappiness))
            worldElevGen.SetLacunarity(gappiness);
        float roughness = (float)(worldElevGen.GetPersistence());
        if (ImGui::InputFloat("Roughness (Persistence)", &roughness))
            worldElevGen.SetPersistence(roughness);
        float extremeness = (float)(worldElevGen.GetFrequency());
        if (ImGui::InputFloat("Extreme-ness (Frequency)", &extremeness))
            worldElevGen.SetFrequency(extremeness);
    }
    if (ImGui::Button("Tiny"))
        this->reGenerate(true);
    if (ImGui::Button("Full"))
        this->reGenerate(false);
    ImGui::LabelText("Tile under mouse", "lon=%d · lat=%d", (int)this->mouseX, (int)this->mouseY);
    ImGui::LabelText("Tile shown", "lon=%d · lat=%d", (int)this->tileX, (int)this->tileY);
    ImGui::End();
}

void MapGenView::onRender(sf::RenderWindow &window) {
    window.draw(this->mapViewFullRect);
    window.draw(this->mapViewTinyRect);
    window.draw(this->mapViewAreaRect);
    window.draw(this->mapViewTileRect);
}

void MapGenView::reGenerate(bool tiny) {
    this->mapViewAreaRect.setTexture(nullptr);
    this->mapViewTileRect.setTexture(nullptr);
    this->tileY = -1.0;
    this->tileY = -1.0;

    clock_t t_start = clock();
    module::ScaleBias elev_scaler;
    elev_scaler.SetSourceModule(0, worldElevGen);
    elev_scaler.SetScale(0.4);

    utils::NoiseMapBuilderSphere builder;
    builder.SetSourceModule(elev_scaler);
    builder.SetDestNoiseMap(this->worldElevMap);
    builder.SetDestSize(tiny ? 1024 : (1 * 4096), tiny ? 512 : (1 * 2048));
    builder.SetBounds(-90.0, 90.0, -180.0, 180.0);
    builder.Build();
    clock_t t_end = clock();
    // ensure full ocean-ness at northern and southern map borders
    for (int y = 0, h = this->worldElevMap.GetHeight(), y_h = h / 8; y < y_h; y++) {
        const float weight = sqrt(sqrt(((float)y / (float)y_h))); // (0 -> force ocean) .. (1 -> keep as-is)
        for (int x = 0, w = this->worldElevMap.GetWidth(); x < w; x++) {
            auto value = this->worldElevMap.GetValue(x, y);
            this->worldElevMap.SetValue(x, y, mix(-1, value, weight));
            value = this->worldElevMap.GetValue(x, h - y);
            this->worldElevMap.SetValue(x, h - y, mix(-1, value, weight));
        }
    }
    // re-scale from actual mins and maxes to true -1..1 with exact 0 remaining equal
    if (!tiny) {
        float height_max = -3.21f, height_min = 3.21f;
        float height_max_new = -3.21f, height_min_new = 3.21f;
        for (int w = this->worldElevMap.GetWidth(), h = this->worldElevMap.GetHeight(), x = 0; x < w; x++)
            for (int y = 0; y < h; y++) {
                const auto height = this->worldElevMap.GetValue(x, y);
                height_max = std::max(height_max, height);
                height_min = std::min(height_min, height);
            }
        const float scale_factor_below = -0.98765431f / height_min; // vs -1.0 this _does_ make THE difference
        const float scale_factor_above = 0.98765431f / height_max;  // for the extreme outlier cases
        for (int w = this->worldElevMap.GetWidth(), h = this->worldElevMap.GetHeight(), x = 0; x < w; x++)
            for (int y = 0; y < h; y++) { // planet of love
                float height = this->worldElevMap.GetValue(x, y);
                height = height * ((height < 0) ? scale_factor_below : scale_factor_above);
                height_max_new = std::max(height_max_new, height);
                height_min_new = std::min(height_min_new, height);
                this->worldElevMap.SetValue(x, y, height);
                if (((y % (h / 8)) == 0) && (((x / 128) % 2) == 0))
                    this->worldElevMap.SetValue(x, y, 1.234f);
            }
        printf("heights done: min=%f->%f,max=%f->%f, %fsec\n", height_min, height_min_new, height_max, height_max_new,
               (float)(t_end - t_start) / CLOCKS_PER_SEC);
        fflush(stdout);
    }

    utils::RendererImage renderer;
    renderer.SetSourceNoiseMap(this->worldElevMap);
    { // coloring
        renderer.ClearGradient();
        renderer.AddGradientPoint(-1.000f, utils::Color(0, 0, 128, 255));  // very-deeps
        renderer.AddGradientPoint(-0.220, utils::Color(0, 0, 255, 255));   // water
        renderer.AddGradientPoint(-0.011, utils::Color(0, 128, 255, 255)); // shoal
        renderer.AddGradientPoint(-0.001, utils::Color(0, 128, 255, 255)); // shoal
        renderer.AddGradientPoint(0.000, utils::Color(128, 128, 32, 255)); // sand
        renderer.AddGradientPoint(0.002, utils::Color(128, 160, 0, 255));  // grass
        renderer.AddGradientPoint(0.125, utils::Color(32, 160, 0, 255));   // grass
        renderer.AddGradientPoint(0.375, utils::Color(128, 128, 128, 255));
        renderer.AddGradientPoint(0.750, utils::Color(160, 160, 160, 255));
        renderer.AddGradientPoint(1.000, utils::Color(192, 192, 192, 255));
        renderer.AddGradientPoint(1.234, utils::Color(255, 96, 0, 255));
        renderer.EnableLight();
        renderer.SetLightContrast(3.21);
        renderer.SetLightBrightness(2.34);
    }
    utils::Image image;
    renderer.SetDestImage(image);
    t_start = clock();
    renderer.Render();

    const auto out_file_path = std::filesystem::absolute("../.local/temp_" + std::string(tiny ? "tiny" : "full") + ".bmp");
    utils::WriterBMP writer;
    writer.SetSourceImage(image);
    writer.SetDestFilename(out_file_path);
    writer.WriteDestFile();
    t_end = clock();

    if (tiny) {
        this->mapViewTinyTex.loadFromFile(out_file_path);
        this->mapViewTinyRect.setTexture(&this->mapViewTinyTex);
        const auto size_tex = this->mapViewTinyTex.getSize();
        this->mapViewTinyRect.setTextureRect(sf::IntRect {0, 0, (int)(size_tex.x), (int)(size_tex.y)});
    } else {
        this->mapViewFullTex.loadFromFile(out_file_path);
        this->mapViewFullRect.setTexture(&this->mapViewFullTex);
        const auto size_tex = this->mapViewFullTex.getSize();
        this->mapViewFullRect.setTextureRect(sf::IntRect {0, 0, (int)(size_tex.x), (int)(size_tex.y)});
        this->mapViewTinyRect.setTextureRect(sf::IntRect {0, 0, (int)(size_tex.x), (int)(size_tex.y)});
        this->mapViewTinyRect.setTexture(&this->mapViewFullTex);
    }

    printf("%s: %fsec\n", out_file_path.c_str(), (float)(t_end - t_start) / CLOCKS_PER_SEC);
    fflush(stdout);
}

void MapGenView::generateTileAndArea() {
    utils::NoiseMapBuilderPlane map_builder;
}
