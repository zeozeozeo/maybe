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
    void load_level();
    void process_event(SDL_Event* event);
    void update(double dt, int screen_w, int screen_h);
    void render(SDL_Renderer* renderer, double dt);
    void reset();
    void init_ui(SDL_Renderer* renderer, SDL_Window* window, float font_scale);
    void draw_ui(double dt);
    void begin_events();
    void end_events();

    bool m_running = true;
    Color m_bg_color = Color(255, 255, 255, 255);
    Level m_level;
    Camera m_camera;
    Player m_player;
    Vec2<int> m_tile_top; // top left tile on the screen (clamped)
    Vec2<int> m_tile_bottom; // bottom right tile on the screen (clamped)
    ParticleSystem m_ps;
    double m_time;
    nk_context* m_ctx;
    bool m_show_ui;
    Ghost m_recording_ghost;
    Ghost m_playback_ghost;
    double m_level_start_time;

    char m_ui_replay_path[256];
    nk_bool m_ui_overwrite_file;
};

#endif // GAME_H
