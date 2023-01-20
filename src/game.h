#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include "color.h"
#include "level.h"
#include "player.h"
#include <SDL.h>

class Game
{
public:
    Game() = default;
    void load_level();
    void process_event(SDL_Event* event);
    void update(double dt, int screen_w, int screen_h);
    void render(SDL_Renderer* renderer, double dt);
    void reset();

    bool m_running = true;
    Color m_bg_color = Color(255, 255, 255, 255);
    Level m_level;
    Camera m_camera;
    Player m_player;
    Vec2<int> m_tile_top; // top left tile on the screen (clamped)
    Vec2<int> m_tile_bottom; // bottom right tile on the screen (clamped)
    ParticleSystem m_ps;
    double m_time;
};

#endif // GAME_H
