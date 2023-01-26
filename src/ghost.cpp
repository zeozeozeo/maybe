#include "ghost.h"
#include "utils.h"

void Ghost::add_action(Vec2<double> pos, bool grounded)
{
    if (m_actions.size() > m_max_actions)
        return;
    m_actions.push_back(Action(Vec2<Sint32>(pos.x, pos.y), grounded));
}

void Ghost::clear_actions()
{
    m_actions.clear();
}

bool Ghost::maybe_add_action(double time, Vec2<double> pos, bool grounded)
{
    if ((time-m_last_added_action_time) >= (1.0 / m_fps)) {
        add_action(pos, grounded);
        m_last_added_action_time = time;
        return true;
    }
    return false;
}

void Ghost::draw(SDL_Renderer* renderer, Camera* camera, double time)
{
    // TODO: emit walking particles (Action has m_grounded, so it
    // shouldn't be that hard)

    if (m_actions.size() == 0)
        return;

    auto time_at_action = [&](int idx) -> double {
        return (double)idx * (1.0 / m_fps);
    };

    // get closest action
    double s_per_frame = 1.0 / m_fps;
    int action_idx = utils::clamp<int>(floor(time / s_per_frame), 0, m_actions.size()-1);

    Vec2<int> pos = m_actions.at(action_idx).m_pos;
    SDL_Rect rect;

    int width = utils::tile_size, height = utils::tile_size * 2;
    float alpha = 0.2;

    if (action_idx < m_actions.size()-1) {
        Vec2<int> next_pos = m_actions.at(action_idx + 1).m_pos;

        // normalize the value between the current and the next action
        double curr_action_time = time_at_action(action_idx);
        double next_action_time = time_at_action(action_idx+1);
        double lerp_val = (time-curr_action_time) / (next_action_time-curr_action_time);

        // lerp between the current action and the next action
        int x = round(utils::lerp(pos.x, next_pos.x, lerp_val));
        int y = round(utils::lerp(pos.y, next_pos.y, lerp_val));
        rect = { x, y, width, height };
    } else {
        alpha += (time_at_action(m_actions.size()-1)-time);
        alpha = utils::clamp<float>(alpha, 0.0, 1.0);

        rect = { pos.x, pos.y, width, height };
    }

    camera->translate(&rect);

    // draw
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha * 255);
    SDL_RenderFillRect(renderer, &rect);
}
