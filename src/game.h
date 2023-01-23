#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include "color.h"
#include "ghost.h"
#include "level.h"
#include "player.h"
#include <SDL.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"

class Game
{
public:
    Game() = default;
    ~Game();
    void load_level();
    void process_event(SDL_Event* event);
    void update(double dt, int screen_w, int screen_h);
    void render(SDL_Renderer* renderer, double dt);
    void reset();
    void init_ui(SDL_Renderer* renderer, SDL_Window* window, float font_scale);
    void draw_ui(double dt);
    void begin_events();
    void end_events();
    void load_textures(SDL_Renderer* renderer);
    void load_texture(std::string path, SDL_Renderer* renderer, SDL_Texture** tex);
    void draw_background(SDL_Renderer* renderer);

    bool m_running = true;
    Color m_bg_color = Color(255, 255, 255, 255);
    Level m_level;
    Camera m_camera;
    Player m_player;
    ParticleSystem m_ps;
    double m_time;
    nk_context* m_ctx;
    bool m_show_ui;
    Ghost m_recording_ghost;
    Ghost m_playback_ghost;
    double m_level_start_time;
    int m_screen_w, m_screen_h;

    char m_ui_replay_path[256];
    nk_bool m_ui_overwrite_file;
    SDL_Texture* m_spike_texture;
};

#endif // GAME_H
