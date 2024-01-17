#include <cstdio>

#include <SDL2/SDL.h>
#include <WickedEngine.h>


void mainLoop(wi::Application&);

int main() {
    wi::renderer::SetRaytracedShadowsEnabled(true);
    wi::renderer::SetShaderSourcePath("../libdeps/WickedEngine/WickedEngine/shaders/");
    wi::renderer::SetShaderPath("../.cache/shaders/");

    wi::Application app;
    app.infoDisplay.active = true;
    app.infoDisplay.watermark = true;
    app.infoDisplay.resolution = true;
    app.infoDisplay.fpsinfo = true;

    sdl2::sdlsystem_ptr_t sdl_sys = sdl2::make_sdlsystem(SDL_INIT_EVERYTHING | SDL_INIT_EVENTS);
    sdl2::window_ptr_t sdl_win =
        sdl2::make_window("mo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 960,
                          SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS
                          //   | SDL_WINDOW_RESIZABLE
        );

    app.SetWindow(sdl_win.get());
    app.Initialize();
    wi::initializer::InitializeComponentsImmediate();

    // wi::eventhandler::SetVSync(false);
    wi::RenderPath3D wi_render_path;
    wi_render_path.setFXAAEnabled(true);
    app.ActivatePath(&wi_render_path);

    const std::string ws01 = "animation_test.wiscene"; // !
    const std::string ws02 = "blend_instancing_test.wiscene";
    const std::string ws03 = "bloom_test.wiscene"; // !
    const std::string ws04 = "cameras.wiscene";    // !
    const std::string ws05 = "cloth_test.wiscene";
    const std::string ws06 = "cube.wiscene"; // !
    const std::string ws07 = "emitter_depthcollision_test.wiscene";
    const std::string ws08 = "emitter_distortion.wiscene";
    const std::string ws09 = "emitter_fire.wiscene";
    const std::string ws10 = "emitter_fluid_50k.wiscene"; // !
    const std::string ws11 = "emitter_rain.wiscene";
    const std::string ws12 = "emitter_skinned.wiscene";
    const std::string ws13 = "emitter_smoke.wiscene";
    const std::string ws14 = "hairparticle_torus.wiscene";
    const std::string ws15 = "hologram_test.wiscene";
    const std::string ws16 = "lensflare_light.wiscene";
    const std::string ws17 = "morph_target_animation_test.wiscene";
    const std::string ws18 = "physics_test.wiscene";
    const std::string ws19 = "shadows_test.wiscene";
    const std::string ws20 = "shadowspriority_point_test.wiscene";
    const std::string ws21 = "shadowspriority_spot_test.wiscene";
    const std::string ws22 = "sound3D_test.wiscene";
    const std::string ws23 = "suzanne.wiscene";
    const std::string ws24 = "teapot.wiscene";
    const std::string ws25 = "volumetric_test.wiscene";
    const std::string ws26 = "water_plane.wiscene";
    const std::string ws27 = "water_test.wiscene";
    const std::string wi_scene_path = "/home/_/c/c/mo/libdeps/WickedEngine/Content/models/" + ws27;
    wi::scene::LoadModel(wi_scene_path);
    wi::scene::GetScene();

    mainLoop(app);
    SDL_Quit();
    return 0;
}

void mainLoop(wi::Application& app) {
    SDL_Event sdl_evt;

    bool quit = false;
    while (!quit) {
        SDL_PumpEvents();
        app.Run();

        while (SDL_PollEvent(&sdl_evt))
            switch (sdl_evt.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_WINDOWEVENT:
                    switch (sdl_evt.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            quit = true;
                            exit(0); // TODO: remove when no more segfault-on-quit turanszkij/WickedEngine/issues/792
                            break;
                        case SDL_WINDOWEVENT_RESIZED:
                            app.SetWindow(app.window);
                            break;
                        case SDL_WINDOWEVENT_FOCUS_LOST:
                            app.is_window_active = false;
                            break;
                        case SDL_WINDOWEVENT_FOCUS_GAINED:
                            app.is_window_active = true;
                            break;
                    }
                    break;
                default:
                    break;
            }
        wi::input::sdlinput::ProcessEvent(sdl_evt);
    }
}
