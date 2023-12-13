#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <map>

#include <imgui.h>
#include <math.h>
#include <misc/cpp/imgui_stdlib.h>
#include <string>

#include "./mapgenview.h"
#include "billow.h"
#include "libnoise_utils/noiseutils.h"
#include "ridgedmulti.h"
#include "scalebias.h"
#include "turbulence.h"


const int worldWidthKm = 4096;
const int worldHeightKm = 2048;
const int worldTileSize = 4096;
const int worldElevDistanceM = 10;
const float worldDisplayScale = 0.75;


void noiseMapToBitmapFile(std::filesystem::path outFilePath, utils::NoiseMap mapElevs, bool bw0To1);
void noiseMapFromBitmapFileBw(utils::NoiseMap* dest, sf::Image* imgBw);

float mix(float x, float y, float a) {
    return (x * (1.0f - a)) + (y * a);
}
float frac(float n) {
    return n - floorf(n);
}
float smoothstep(float edge0, float edge1, float x) {
    // Scale, bias and saturate x to 0..1 range
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3.0f - 2.0f * x);
}
float interpWeight(float x, float y) {
    return (x * x) * (y * y) * (9 - 6 * x - 6 * y + 4 * x * y);
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
    this->worldElevGen.SetFrequency(1.212);
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
         }) {
        rect_and_tex.second->setOrigin(0, 0);
        rect_and_tex.second->setFillColor(sf::Color::White);
        rect_and_tex.first->setSmooth(true);
        rect_and_tex.first->setRepeated(false);
    }
    this->mapViewTileTex.setSmooth(false);

    this->mapViewFullRect.setPosition(0, 512 + 64);
    this->mapViewFullRect.setSize({(worldDisplayScale * (float)worldWidthKm), (worldDisplayScale * (float)worldHeightKm)});

    this->mapViewTinyRect.setPosition(0, 0);
    this->mapViewTinyRect.setSize({1024, 512});

    this->mapViewTileRect.setPosition(1024 + 64, 0);
    this->mapViewTileRect.setSize({512, 512});

    {
        const auto bmpFilePath = std::filesystem::absolute("../.local/temp_bill.bmp");
        sf::Image img;
        if (img.loadFromFile(bmpFilePath))
            noiseMapFromBitmapFileBw(&this->repBillow, &img);
        else {
            noise::module::Billow billow;
            billow.SetFrequency(0.0123);
            billow.SetNoiseQuality(NoiseQuality::QUALITY_BEST);
            billow.SetLacunarity(0.1 * worldElevGen.GetLacunarity());
            billow.SetPersistence(0.1 * worldElevGen.GetPersistence());
            billow.SetSeed(worldElevGen.GetSeed());
            this->prepTileableLayer(billow, this->repBillow, bmpFilePath);
        }
    }
    {
        const auto bmpFilePath = std::filesystem::absolute("../.local/temp_ridg.bmp");
        sf::Image img;
        if (img.loadFromFile(bmpFilePath))
            noiseMapFromBitmapFileBw(&this->repRidged, &img);
        else {
            noise::module::RidgedMulti ridged;
            ridged.SetFrequency(0.005);
            ridged.SetNoiseQuality(NoiseQuality::QUALITY_BEST);
            ridged.SetSeed(worldElevGen.GetSeed());
            this->prepTileableLayer(ridged, this->repRidged, bmpFilePath);
        }
    }
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
        ImGui::InputText("Map Name", &this->seedName);
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
        this->generateTile();
    ImGui::End();
}

void MapGenView::onRender(sf::RenderWindow &window) {
    window.draw(this->mapViewFullRect);
    window.draw(this->mapViewTinyRect);
    window.draw(this->mapViewTileRect);
    window.draw(this->tileSelRect);
}

void MapGenView::reGenerate(bool tiny) {
    auto ascii = std::string(this->seedName);
    std::transform(ascii.begin(), ascii.end(), ascii.begin(), toascii);
    auto lower = std::string(ascii);
    std::transform(lower.begin(), lower.end(), lower.begin(), tolower);
    auto upper = std::string(lower);
    std::transform(upper.begin(), upper.end(), upper.begin(), toupper);
    std::hash<std::string> hasher;
    auto hash = hasher(upper + lower);
    this->worldElevGen.SetSeed((int)hash);

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
        const float scale_factor_below = -0.987654321f / height_min;
        const float scale_factor_above = 0.55f / height_max;
        for (int w = this->worldElevMap.GetWidth(), h = this->worldElevMap.GetHeight(), x = 0; x < w; x++)
            for (int y = 0; y < h; y++) {
                float elev = this->worldElevMap.GetValue(x, y);
                elev = elev * ((elev < 0) ? scale_factor_below : scale_factor_above);
                const float ridged = 0.44f * this->repRidged.GetValue(x, y);
                const float billow = 0.44f * this->repBillow.GetValue(x, y);

                float elev_m = elev * 1000.0f;
                elev = elev + mix(0.0f, mix(billow, ridged, elev + 0.44f), std::clamp(elev_m, 0.0f, 1.0f));
                height_max_new = std::max(height_max_new, elev);
                height_min_new = std::min(height_min_new, elev);
                this->worldElevMap.SetValue(x, y, elev);

                // clim-zone divider lines (for visualization purposes only)
                if (((y % (h / 8)) == 0) && (((x / 128) % 2) == 0))
                    this->worldElevMap.SetValue(x, y, 1.234f);
            }
        printf("heights done: min=%f->%f,max=%f->%f, %fsec\n", height_min, height_min_new, height_max, height_max_new,
               (float)(t_end - t_start) / CLOCKS_PER_SEC);
        fflush(stdout);
    }

    const auto out_file_path = std::filesystem::absolute("../.local/temp_" + std::string(tiny ? "tiny" : "full") + ".bmp");
    t_start = clock();
    noiseMapToBitmapFile(out_file_path, this->worldElevMap, false);
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

void MapGenView::generateTile() {
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
            float x_frac = x_here - x_less, y_frac = y_here - y_less;

            // codeproject.com/Articles/5312360/2-D-Interpolation-Functions "constrained bicubic"
            float elev = interpWeight(1 - x_frac, 1 - y_frac) * worldElevMap.GetValue(x_less, y_less)
                         + interpWeight(x_frac, 1 - y_frac) * worldElevMap.GetValue(x_more, y_less)
                         + interpWeight(1 - x_frac, y_frac) * worldElevMap.GetValue(x_less, y_more)
                         + interpWeight(x_frac, y_frac) * worldElevMap.GetValue(x_more, y_more);

            elev_tile.SetValue(x, y, (float)elev);
        }
    }

    const auto out_file_path_tile = std::filesystem::absolute("../.local/temp_tile.bmp");
    noiseMapToBitmapFile(out_file_path_tile, elev_tile, false);

    this->mapViewTileTex.loadFromFile(out_file_path_tile);
    this->mapViewTileRect.setTexture(&this->mapViewTileTex);
    const auto size_tex_tile = this->mapViewTileTex.getSize();
    this->mapViewTileRect.setTextureRect(sf::IntRect {0, 0, (int)(size_tex_tile.x), (int)(size_tex_tile.y)});
    fflush(stdout);
}

void MapGenView::prepTileableLayer(module::Module &module, utils::NoiseMap &destMap, std::string outFilePath) {
    utils::NoiseMapBuilderPlane builder;
    builder.SetDestNoiseMap(destMap);
    builder.SetDestSize(worldTileSize, worldTileSize);
    builder.SetBounds(0, worldTileSize, 0, worldTileSize);
    noise::module::Turbulence turb;
    turb.SetSourceModule(0, module);
    turb.SetPower(1);
    turb.SetFrequency(4.0);
    turb.SetSeed(worldElevGen.GetSeed());
    builder.SetSourceModule(turb);
    builder.EnableSeamless(true);
    builder.Build();

    float elev_max = -3.21f, elev_min = 3.21f;
    for (int x = 0; x < worldTileSize; x++)
        for (int y = 0; y < worldTileSize; y++) {
            float value = destMap.GetValue(x, y);
            elev_max = std::max(value, elev_max);
            elev_min = std::min(value, elev_min);
        }
    float elev_range = elev_max - elev_min;
    for (int x = 0; x < worldTileSize; x++)
        for (int y = 0; y < worldTileSize; y++) {
            float elev = destMap.GetValue(x, y);
            elev = elev - elev_min;
            elev = elev / elev_range;
            if (elev < 0 || elev > 1) {
                printf("%f\n", elev);
            }
            destMap.SetValue(x, y, elev);
        }

    fflush(stdout);
    noiseMapToBitmapFile(outFilePath, destMap, true);
}

void noiseMapToBitmapFile(std::filesystem::path outFilePath, utils::NoiseMap mapElevs, bool bw0To1) {
    utils::RendererImage renderer;
    renderer.SetSourceNoiseMap(mapElevs);
    if (!bw0To1) { // coloring
        renderer.ClearGradient();
        renderer.AddGradientPoint(-1.000f, utils::Color(0, 0, 128, 255));  // very-deeps
        renderer.AddGradientPoint(-0.222, utils::Color(0, 0, 255, 255));   // water
        renderer.AddGradientPoint(-0.022, utils::Color(0, 96, 255, 255));  // shoal
        renderer.AddGradientPoint(-0.002, utils::Color(0, 128, 255, 255)); // shoal
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
    } else {
        renderer.ClearGradient();
        renderer.AddGradientPoint(0, utils::Color(0, 0, 0, 255));
        renderer.AddGradientPoint(1, utils::Color(255, 255, 255, 255));
    }
    utils::Image image;
    renderer.SetDestImage(image);
    renderer.Render();

    utils::WriterBMP writer;
    writer.SetSourceImage(image);
    writer.SetDestFilename(outFilePath);
    writer.WriteDestFile();
}

void noiseMapFromBitmapFileBw(utils::NoiseMap* dest, sf::Image* imgBw) {
    const auto size_img = imgBw->getSize();
    dest->SetSize(worldTileSize, worldTileSize);
    int y_map = worldTileSize - 1;
    for (uint y = 0; y < size_img.y; y++) {
        for (uint x = 0; x < size_img.x; x++) {
            const auto bw_rgb = imgBw->getPixel(x, y);
            const auto bw = (((float)bw_rgb.r) + ((float)bw_rgb.g) + ((float)bw_rgb.b)) / 3.0f;
            const float elev = bw / 255.0f;
            dest->SetValue(x, y_map, elev);
        }
        y_map--;
    }
}
