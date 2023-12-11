#include <cstdio>
#include <filesystem>
#include <imgui.h>

#include "./libnoise_utils/noiseutils.h"

#include "./mapgenview.h"
#include "perlin.h"


MapGenView::MapGenView() {
    this->perlinNoise.SetLacunarity(2.222);
    this->perlinNoise.SetOctaveCount(30);
    this->perlinNoise.SetSeed(12);
    this->previewTinyRect.setSize({512, 256});
    this->previewFullRect.setSize({3072, 1536});
    this->previewTinyRect.setOrigin(0, 0);
    this->previewFullRect.setOrigin(0, 0);
    this->previewTinyRect.setPosition(768, 0);
    this->previewFullRect.setPosition(0, 320);
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
    perlinNoise.SetNoiseQuality(tiny ? NoiseQuality::QUALITY_FAST : NoiseQuality::QUALITY_BEST);
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderSphere heightMapBuilder;
    heightMapBuilder.SetSourceModule(perlinNoise);
    heightMapBuilder.SetDestNoiseMap(heightMap);
    heightMapBuilder.SetDestSize(tiny ? 512 : 3072, tiny ? 256 : 1536);
    heightMapBuilder.SetBounds(-90.0, 90.0, -180.0, 180.0);
    heightMapBuilder.Build();

    utils::RendererImage renderer;
    utils::Image image;
    renderer.SetSourceNoiseMap(heightMap);
    renderer.SetDestImage(image);
    { // coloring
        renderer.ClearGradient();
        renderer.AddGradientPoint(-1.0000, utils::Color(0, 0, 128, 255));    // deeps
        renderer.AddGradientPoint(-0.2500, utils::Color(0, 0, 255, 255));    // shallow
        renderer.AddGradientPoint(0.0000, utils::Color(0, 128, 255, 255));   // shore
        renderer.AddGradientPoint(0.0625, utils::Color(240, 240, 64, 255));  // sand
        renderer.AddGradientPoint(0.1250, utils::Color(32, 160, 0, 255));    // grass
        renderer.AddGradientPoint(0.3750, utils::Color(224, 224, 0, 255));   // dirt
        renderer.AddGradientPoint(0.7500, utils::Color(128, 128, 128, 255)); // rock
        renderer.AddGradientPoint(1.0000, utils::Color(255, 255, 255, 255)); // snow
        renderer.EnableLight();
        renderer.SetLightContrast(3.21);
        renderer.SetLightBrightness(2.34);
    }
    renderer.Render();

    const auto out_file_path = std::filesystem::absolute("../.local/temp_" + std::string(tiny ? "tiny" : "full") + ".bmp");
    utils::WriterBMP writer;
    writer.SetSourceImage(image);
    writer.SetDestFilename(out_file_path);
    writer.WriteDestFile();

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

    printf("%fsec\n", ((double_t)(clock())) / ((double_t)(CLOCKS_PER_SEC)));
}
