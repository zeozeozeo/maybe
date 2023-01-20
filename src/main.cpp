#include <SDL.h>
#include <string>
#include "error.h"
#include "game.h"
#include "utils.h"
#include <memory>

#define PHYSICS_FRAMERATE 144.0

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
        error("failed to initialize SDL");
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
        error("failed to create window");

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
                window,
                -1,
                SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr)
        error("failed to create renderer");

    SDL_GL_SetSwapInterval(1); // enable vsync (if the renderer is accelerated)

    // create game context (allocated on the heap incase it gets too big)
    std::unique_ptr<Game> game = std::make_unique<Game>();
    game->load_level();

    // main loop
    Uint64 time_now = SDL_GetPerformanceCounter();
    Uint64 last_frametime = 0;
    double dt = 0.0;

    while (game->m_running) {
        // process SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event))
            game->process_event(&event);

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
