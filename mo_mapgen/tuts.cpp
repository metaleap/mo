#include <cstdio>

#include <noise/noise.h>
#include <noise/module/perlin.h>
#include "noiseutils.h"




void tut2() {
    noise::module::Perlin module_perlin;
    double value = module_perlin.GetValue(14.5, 20.25, 75.75);
    printf("%f\n", value);
}

void tut3() {
    noise::module::Perlin module_perlin;
    utils::NoiseMap height_map;
    utils::NoiseMapBuilderPlane height_map_builder;

    height_map_builder.SetSourceModule(module_perlin);
    height_map_builder.SetDestNoiseMap(height_map);
    height_map_builder.SetDestSize(512, 512);
    height_map_builder.SetBounds(2.0, 6.0, 1.0, 5.0);
    height_map_builder.Build();

    utils::RendererImage renderer;
    utils::Image image;
    renderer.SetSourceNoiseMap(height_map);
    renderer.SetDestImage(image);
    renderer.Render();

    utils::WriterBMP writer_bmp;
    writer_bmp.SetSourceImage(image);
    writer_bmp.SetDestFilename("/home/_/c/c/mo/.local/tut3.bmp");
    writer_bmp.WriteDestFile();
}
