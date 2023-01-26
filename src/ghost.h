#ifndef GHOST_H
#define GHOST_H

#include <SDL.h>
#include "camera.h"
#include "vec2.h"
#include <string>
#include <vector>
#include <fstream>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(assets);

#define GHOST_PLAYBACK_FPS 40.0

class Action
{
public:
    Action(Vec2<Sint32> pos, bool grounded):
        m_pos(pos),
        m_grounded(grounded)
    {}
    Action() = default;
    Vec2<Sint32> m_pos;
    bool m_grounded;
};

class Ghost
{
public:
    Ghost() = default;
    void add_action(Vec2<double> pos, bool grounded);
    void clear_actions();
    bool maybe_add_action(double time, Vec2<double> pos, bool grounded);
    void draw(SDL_Renderer* renderer, Camera* camera, double time);

    std::vector<Action> m_actions;
    double m_fps = GHOST_PLAYBACK_FPS;
private:
    double m_last_added_action_time;
    size_t m_max_actions = (128 * 1024 * 1024) / sizeof(Action);
};

#endif // GHOST_H
