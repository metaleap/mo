#include <cstdio>

#include <SDL2/SDL.h>
#include <WickedEngine.h>


void mainLoop(wi::Application&);

int main() {
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
                          SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN);
    app.SetWindow(sdl_win.get());
    app.Initialize();
    wi::initializer::InitializeComponentsImmediate();

    wi::RenderPath3D wi_render_path;
    app.ActivatePath(&wi_render_path);

    wi::scene::LoadModel("/home/_/c/c/mo/libdeps/WickedEngine/Content/models/water_test.wiscene");
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
                            exit(0); // TODO: remove when no more segfault-on-quit
                            break;
                        case SDL_WINDOWEVENT_RESIZED:
                            app.SetWindow(app.window);
                            break;
                        case SDL_WINDOWEVENT_FOCUS_LOST:
                            break;
                        case SDL_WINDOWEVENT_FOCUS_GAINED:
                            break;
                    }
                    break;
                default:
                    break;
            }
        wi::input::sdlinput::ProcessEvent(sdl_evt);
    }
}
