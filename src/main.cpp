#include <SDL.h>
#include <string>
#include "error.h"
#include "game.h"
#include "utils.h"
#include <memory>

#define PHYSICS_FRAMERATE 144.0

int main()
{
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
        error(std::string("failed to initialize SDL: ") + SDL_GetError());
    utils::init();

    int width = 1280, height = 720;

    // create window
    SDL_Window* window = SDL_CreateWindow(
                    "maybe",
                    SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED,
                    width, height,
                    SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == nullptr)
        error(std::string("failed to create window: ") + SDL_GetError());

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
                window,
                -1,
                SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr)
        error(std::string("failed to create renderer: ") + SDL_GetError());

    SDL_GL_SetSwapInterval(1); // enable vsync (if the renderer is accelerated)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // scale the renderer output for high-DPI displays
    float font_scale = 1.0;
    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
        SDL_GetWindowSize(window, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(renderer, scale_x, scale_y);
        font_scale = scale_y;
    }

    // create game context (allocated on the heap incase it gets too big)
    std::unique_ptr<Game> game = std::make_unique<Game>();

    game->load_textures(renderer);
    game->init_ui(renderer, window, font_scale);
    game->load_level();

    // main loop
    Uint64 time_now = SDL_GetPerformanceCounter();
    Uint64 last_frametime = 0;
    double dt = 0.0;

    while (game->m_running) {
        // process SDL events
        SDL_Event event;

        game->begin_events();
        while (SDL_PollEvent(&event))
            game->process_event(&event);
        game->end_events();

        // calculate delta time (in seconds)
        last_frametime = time_now;
        time_now = SDL_GetPerformanceCounter();
        dt = (double)((time_now - last_frametime)*1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;
        double fps = 1.0 / dt;

        // render
        int screen_w, screen_h;
        SDL_GetWindowSizeInPixels(window, &screen_w, &screen_h);

        // we might have to run the physics step multiple times
        // per frame at low framerates
        int steps = fps >= PHYSICS_FRAMERATE ? 1 : floor(PHYSICS_FRAMERATE / fps);

        for (int i = 0; i < steps; i++)
            game->update(dt / steps, screen_w, screen_h);

        game->render(renderer, dt);
    }

    // cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
