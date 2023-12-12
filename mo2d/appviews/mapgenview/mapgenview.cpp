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
#include <math.h>
#include <misc/cpp/imgui_stdlib.h>
#include <string>

#include "./mapgenview.h"
#include "libnoise_utils/noiseutils.h"


const int worldWidthKm = 4096;
const int worldHeightKm = 2048;
const int worldTileSize = 4096;
const int worldElevDistanceM = 10;
const float worldDisplayScale = 0.75;


void noiseMapToBitmapFile(std::filesystem::path outFilePath, utils::NoiseMap mapElevs);

float mix(float x, float y, float a) {
    return (x * (1.0f - a)) + (y * a);
}
float smoothstep(float e0, float e1, float a) {
    a = std::clamp((a - e0) / (e1 - e0), 0.0f, 1.0f);
    return a * a * (3.0f - (2.0f * a));
}


MapGenView::MapGenView() {
    const auto num_tiles_x = ((worldWidthKm * 1000) / worldElevDistanceM) / worldTileSize /*tile size*/;
    const auto num_tiles_y = ((worldHeightKm * 1000) / worldElevDistanceM) / worldTileSize /*tile size*/;
    this->numTiles = {num_tiles_x, num_tiles_y};
    this->worldElevGen.SetNoiseQuality(NoiseQuality::QUALITY_BEST);
    this->worldElevGen.SetLacunarity(2.121);
    this->worldElevGen.SetOctaveCount(30);
    this->worldElevGen.SetSeed(12);
    this->worldElevGen.SetPersistence(0.515);
    this->worldElevGen.SetFrequency(1.221);
    this->tileSelRect.setFillColor(sf::Color(255, 255, 255, 64));
    this->tileSelRect.setOutlineColor(sf::Color(255, 255, 255, 255));
    this->tileSelRect.setOutlineThickness(1.0);
    this->tileSelRect.setOrigin(0, 0);
    this->tileSelRect.setPosition(0, 512 + 16);
    this->tileSelRect.setSize({((worldDisplayScale * (float)worldWidthKm) / (float)num_tiles_x),
                               ((worldDisplayScale * (float)worldHeightKm) / (float)num_tiles_y)});
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

    this->mapViewFullRect.setPosition(0, 512 + 64);
    this->mapViewFullRect.setSize({(worldDisplayScale * (float)worldWidthKm), (worldDisplayScale * (float)worldHeightKm)});

    this->mapViewTinyRect.setPosition(0, 0);
    this->mapViewTinyRect.setSize({1024, 512});

    this->mapViewAreaRect.setPosition(1024 + 64, 0);
    this->mapViewAreaRect.setSize({512, 512});

    this->mapViewTileRect.setPosition(1024 + 64 + 512 + 64, 0);
    this->mapViewTileRect.setSize({512, 512});
}

void MapGenView::onInput(const sf::Event &evt) {
    const auto pos_mapview = mapViewFullRect.getPosition();
    if (evt.type == sf::Event::MouseMoved) {
        auto mx = (float)evt.mouseMove.x, my = (float)evt.mouseMove.y;
        const auto size_mapview = mapViewFullRect.getSize();
        if ((mx >= pos_mapview.x) && (mx <= (pos_mapview.x + size_mapview.x)) && (my >= pos_mapview.y)
            && (my <= (pos_mapview.y + size_mapview.y))) {
            this->mouseX = (mx - pos_mapview.x) / size_mapview.x;          // 0..1
            this->mouseY = 1.0f - ((my - pos_mapview.y) / size_mapview.y); // 0..1
        } else {
            this->mouseX = -1.0;
            this->mouseY = -1.0;
        }
    }
    if ((evt.type == sf::Event::MouseButtonReleased)) {
        if ((this->mouseX >= 0) && (this->mouseX < 1.0) && ((this->mouseY >= 0) && (this->mouseY < 1.0))) {
            this->tileX = (int)(this->mouseX * (float)this->numTiles.x);
            this->tileY = (int)(this->mouseY * (float)this->numTiles.y);
            const auto size_rect = mapViewFullRect.getSize();
            auto sel_tile_x = size_rect.x * ((float)this->tileX / (float)this->numTiles.x);
            auto sel_tile_y =
                (size_rect.y * (1.0f - (((float)this->tileY) / (float)this->numTiles.y))) - tileSelRect.getSize().y;
            tileSelRect.setPosition(pos_mapview.x + sel_tile_x, pos_mapview.y + sel_tile_y - 1.0f);
        }
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
    ImGui::LabelText("Mouse", "x=%.2f , y=%.2f", this->mouseX, this->mouseY);
    if (ImGui::Button((std::to_string(this->tileX) + "," + std::to_string(this->tileY)).c_str()))
        this->generateTileOrArea();
    ImGui::End();
}

void MapGenView::onRender(sf::RenderWindow &window) {
    window.draw(this->mapViewFullRect);
    window.draw(this->mapViewTinyRect);
    window.draw(this->mapViewAreaRect);
    window.draw(this->mapViewTileRect);
    window.draw(this->tileSelRect);
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
    builder.SetDestSize(tiny ? 512 : worldWidthKm, tiny ? 256 : worldHeightKm);
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

    const auto out_file_path = std::filesystem::absolute("../.local/temp_" + std::string(tiny ? "tiny" : "full") + ".bmp");
    t_start = clock();
    noiseMapToBitmapFile(out_file_path, this->worldElevMap);
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

void MapGenView::generateTileOrArea() {
    if ((this->tileX < 0) || (this->tileX >= this->numTiles.x) || (this->tileY < 0) || (this->tileY >= this->numTiles.y))
        return;
    utils::NoiseMap elev_tile;
    elev_tile.SetSize(worldTileSize, worldTileSize);

    float x_full = ((float)worldWidthKm) * ((float)this->tileX / (float)this->numTiles.x);
    float y_full = ((float)worldHeightKm) * ((float)this->tileY / (float)this->numTiles.y);
    float x_num = ((float)worldWidthKm) / ((float)this->numTiles.x);
    float y_num = ((float)worldHeightKm) / ((float)this->numTiles.y);
    for (int x = 0; x < worldTileSize; x++) {
        float x_off = (x_num / (float)worldTileSize) * (float)x;
        for (int y = 0; y < worldTileSize; y++) {
            float y_off = (y_num / (float)worldTileSize) * (float)y;
            float x_here = x_full + x_off, y_here = y_full + y_off;
            float x_less = floorf(x_here), x_more = ceilf(x_here);
            float y_less = floorf(y_here), y_more = ceilf(y_here);

            float elev1 = this->worldElevMap.GetValue(x_less, y_less);
            float dist1 = sqrt((x_less * x_less) + (y_less * y_less));
            float elev2 = this->worldElevMap.GetValue(x_more, y_less);
            float dist2 = sqrt((x_more * x_more) + (y_less * y_less));
            float elev3 = this->worldElevMap.GetValue(x_less, y_more);
            float dist3 = sqrt((x_less * x_less) + (y_more * y_more));
            float elev4 = this->worldElevMap.GetValue(x_more, y_more);
            float dist4 = sqrt((x_more * x_more) + (y_more * y_more));

            if (x == 2000 && y == 2000) {
                printf("e1=%.3f d1=%.1f\n", elev1, dist1);
                printf("e2=%.3f d2=%.1f\n", elev2, dist2);
                printf("e3=%.3f d3=%.1f\n", elev3, dist3);
                printf("e4=%.3f d4=%.1f\n", elev4, dist4);
            }

            elev_tile.SetValue(x, y, 0.25f * (elev1 + elev2 + elev3 + elev4));
        }
    }

    const auto out_file_path_tile = std::filesystem::absolute("../.local/temp_tile.bmp");
    noiseMapToBitmapFile(out_file_path_tile, elev_tile);

    this->mapViewTileTex.loadFromFile(out_file_path_tile);
    this->mapViewTileRect.setTexture(&this->mapViewTileTex);
    const auto size_tex_tile = this->mapViewTileTex.getSize();
    this->mapViewTileRect.setTextureRect(sf::IntRect {0, 0, (int)(size_tex_tile.x), (int)(size_tex_tile.y)});
    fflush(stdout);
}

void noiseMapToBitmapFile(std::filesystem::path outFilePath, utils::NoiseMap mapElevs) {
    utils::RendererImage renderer;
    renderer.SetSourceNoiseMap(mapElevs);
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
    renderer.Render();

    utils::WriterBMP writer;
    writer.SetSourceImage(image);
    writer.SetDestFilename(outFilePath);
    writer.WriteDestFile();
}
