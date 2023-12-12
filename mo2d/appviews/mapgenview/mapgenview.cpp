#include <cstdio>
#include <filesystem>
#include <imgui.h>

#include "./libnoise_utils/noiseutils.h"

#include "./mapgenview.h"
#include "perlin.h"


MapGenView::MapGenView() {
    this->perlinNoise.SetLacunarity(2.111);
    this->perlinNoise.SetOctaveCount(30);
    this->perlinNoise.SetSeed(12);
    this->finalTerrain.SetBounds(-1000, 10); // full perlin range is "usually but not guaranteed always" -1..1
    // this->finalTerrain.SetEdgeFalloff(0.11);

    this->previewTinyRect.setSize({1024, 512});
    this->previewFullRect.setSize({3072, 1536});
    this->previewTinyRect.setOrigin(0, 0);
    this->previewFullRect.setOrigin(0, 0);
    this->previewTinyRect.setPosition(768, 0);
    this->previewFullRect.setPosition(0, 624);
    this->previewTinyRect.setFillColor(sf::Color::White);
    this->previewFullRect.setFillColor(sf::Color::White);
}

void MapGenView::onUpdate(sf::Time) {
    ImGui::Begin("MapGen");
    {
        int seed = perlinNoise.GetSeed();
        if (ImGui::InputInt("Seed", &seed))
            perlinNoise.SetSeed(seed);
        int num_octaves = perlinNoise.GetOctaveCount();
        if (ImGui::InputInt("Octaves (6-30)", &num_octaves))
            perlinNoise.SetOctaveCount(num_octaves);
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

    module::RidgedMulti ridged;
    ridged.SetNoiseQuality(NoiseQuality::QUALITY_BEST);
    ridged.SetFrequency(perlinNoise.GetFrequency());
    ridged.SetLacunarity(perlinNoise.GetLacunarity());
    ridged.SetOctaveCount(perlinNoise.GetOctaveCount());
    ridged.SetSeed(perlinNoise.GetSeed());

    this->finalTerrain.SetSourceModule(0, ridged);
    this->finalTerrain.SetSourceModule(1, perlinNoise);
    this->finalTerrain.SetControlModule(perlinNoise);

    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderSphere heightMapBuilder;
    heightMapBuilder.SetSourceModule(perlinNoise);
    heightMapBuilder.SetDestNoiseMap(heightMap);
    heightMapBuilder.SetDestSize(tiny ? 512 : 2048, tiny ? 256 : 1024);
    heightMapBuilder.SetBounds(-90.0, 90.0, -180.0, 180.0);
    heightMapBuilder.Build();
    clock_t t_end = clock();
    printf("heights done: %fsec\n", (float)(t_end - t_start) / CLOCKS_PER_SEC);

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
        renderer.AddGradientPoint(1.111, utils::Color(255, 255, 255, 255));
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

    printf("image done: %fsec\n", (float)(t_end - t_start) / CLOCKS_PER_SEC);
}
