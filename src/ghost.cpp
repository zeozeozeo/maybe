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
    if ((time - m_last_added_action_time) >= (1.0 / m_fps)) {
        add_action(pos, grounded);
        m_last_added_action_time = time;
        return true;
    }
    return false;
}

void Ghost::save_to_file(std::string path, bool overwrite)
{
    if (!overwrite) {
        // check if file exists
        std::ifstream file(path);
        if (file.good()) {
            SDL_Log("ghost: \"%s\" already exists!", path.c_str());
            return;
        }
    }

    std::ofstream file;
    file.open(path, std::iostream::out | std::iostream::binary);

    // write fps
    file.write(reinterpret_cast<const char*>(&m_fps), sizeof(m_fps));

    for (auto& action : m_actions) {
        // write x and y values
        file.write(reinterpret_cast<const char*>(&action.m_pos.x), sizeof(action.m_pos.x));
        file.write(reinterpret_cast<const char*>(&action.m_pos.y), sizeof(action.m_pos.y));

        Uint8 is_grounded = action.m_grounded;
        file.write(reinterpret_cast<const char*>(&is_grounded), sizeof(is_grounded));
    }

    file.close();
    SDL_Log("ghost: saved replay to file \"%s\" (%zu actions)", path.c_str(), m_actions.size());
}

void Ghost::load_from_file_e(std::string path)
{
    auto fs = cmrc::assets::get_filesystem();
    cmrc::file file;

    try {
        file = fs.open(ASSETS_PATH + path);
    } catch (std::system_error) {
        SDL_Log("ghost: file \"%s\" not found", path.c_str());
        return;
    }

    if (file.size() < 1) {
        SDL_Log("ghost: invalid file \"%s\"", path.c_str());
        return;
    }

    const char* data = file.begin();

    clear_actions();

    Vec2<double> current_action;
    bool grounded = false;

    // read fps
    m_fps = *reinterpret_cast<const double*>(&data[0]);

    for (size_t i = sizeof(m_fps), val_idx = 0; i < file.size();) {
        int param = val_idx % 3;

        if (param == 0 && val_idx > 0)
            add_action(current_action, grounded);
        if (param <= 1) {
            // x and y position
            Sint32 val = *reinterpret_cast<const Sint32*>(&data[i]);

            if (param == 0)
                current_action.x = val;
            else if (param == 1)
                current_action.y = val;

            i += sizeof(val);
        } else {
            // grounded
            Uint8 val = *reinterpret_cast<const Uint8*>(&data[i]);
            grounded = val;
            i += sizeof(val);
        }

        val_idx++;
    }

    SDL_Log("ghost: loaded replay from file %s (%zu actions, %f fps)",
            path.c_str(),
            m_actions.size(),
            m_fps);
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
        rect = { pos.x, pos.y, width, height };
    }

    camera->translate(&rect);

    // draw
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);
    SDL_RenderFillRect(renderer, &rect);
}
