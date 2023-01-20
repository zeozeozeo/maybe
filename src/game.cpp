#include "game.h"
#include "utils.h"

void Game::process_event(SDL_Event* event)
{
    SDL_Scancode scancode;

    switch (event->type) {
    case SDL_QUIT:
        m_running = false;
        break;
    case SDL_KEYDOWN:
        if (event->key.repeat)
            break;
        scancode = event->key.keysym.scancode;

        switch (scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_SPACE:
            if (m_player.m_jumps_since_landed > 0)
                m_player.jump();
            break;
        case SDL_SCANCODE_R:
            reset();
            break;
        default:
            break;
        }
        break;
    }
}

void Game::update(double dt, int screen_w, int screen_h) {
    m_time += dt;
    m_player.update(dt, m_time, &m_ps);

    // the positions of tiles that are visible on the screen (from top-left to bottom-right)
    m_tile_top.x = utils::clamp<int>(m_camera.m_pos.x / utils::tile_size - 2, 0, m_level.m_width);
    m_tile_top.y = utils::clamp<int>(m_camera.m_pos.y / utils::tile_size - 2, 0, m_level.m_height);
    m_tile_bottom.x = utils::clamp<int>((m_camera.m_pos.x + screen_w) / utils::tile_size + 2, 0, m_level.m_width);
    m_tile_bottom.y = utils::clamp<int>((m_camera.m_pos.y + screen_h) / utils::tile_size + 2, 0, m_level.m_height);

    m_player.collide(&m_level, m_tile_top, m_tile_bottom, &m_ps);

    Vec2 camera_target(m_player.m_pos.x + m_player.m_size.x / 2,
                       m_player.m_pos.y + m_player.m_size.y / 2);
    m_camera.update(camera_target, screen_w, screen_h);
}

void Game::render(SDL_Renderer* renderer, double dt)
{
    SDL_SetRenderDrawColor(renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, m_bg_color.a);
    SDL_RenderClear(renderer);

    // draw level
    for (int x = m_tile_top.x; x < m_tile_bottom.x; x++) {
        for (int y = m_tile_top.y; y < m_tile_bottom.y; y++) {
            // would be cool to check for collisions here,
            // since we're doing exactly the same thing there,
            // but the physics loop has to be separated from the
            // draw loop :(
            Tile* tile = m_level.tile_at(x, y);

            switch (tile->m_type) {
            case SOLID:
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                break;
            default:
                continue;
            }

            SDL_Rect rect = m_level.tile_to_rect(x, y);
            m_camera.translate(&rect);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // draw particles
    m_ps.draw(dt, &m_camera, renderer);

    // draw player
    SDL_Rect player_rect = m_player.get_rect();
    m_camera.translate(&player_rect);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player_rect);

    SDL_RenderPresent(renderer);
}

void Game::load_level()
{
    m_level = Level("level1.png");
    reset();
}

void Game::reset()
{
    utils::tile_size = 64;
    m_player = Player(); // reset player
    m_player.m_pos = Vec2<double>(m_level.m_spawn_pos.x * utils::tile_size,
                                 (m_level.m_spawn_pos.y-1) * utils::tile_size);
}
