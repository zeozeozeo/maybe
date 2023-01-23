#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

#include "game.h"
#include "error.h"
#include "utils.h"

#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

void Game::init_ui(SDL_Renderer* renderer, SDL_Window* window, float font_scale)
{
    m_ctx = nk_sdl_init(window, renderer);

    struct nk_font_atlas *atlas;
    struct nk_font_config config = nk_font_config(0);
    struct nk_font *font;

    // set up the font atlas and add desired font; note that font sizes are
    // multiplied by font_scale to produce better results at higher DPIs
    nk_sdl_font_stash_begin(&atlas);
    font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
    nk_sdl_font_stash_end();

    // this hack makes the font appear to be scaled down to the desired
    // size and is only necessary when font_scale > 1
    font->handle.height /= font_scale;
    // nk_style_load_all_cursors(m_ctx, atlas->cursors);
    nk_style_set_font(m_ctx, &font->handle);
}

void Game::draw_ui(double dt)
{
    if (nk_begin(m_ctx, "Debug", nk_rect(50, 50, 350, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        if (nk_tree_push(m_ctx, NK_TREE_TAB, "Ghost", NK_MINIMIZED)) {
            nk_layout_row_dynamic(m_ctx, 15, 1);
            nk_labelf(m_ctx, NK_TEXT_LEFT, "recorded actions: %zu", m_recording_ghost.m_actions.size());
            nk_labelf(m_ctx, NK_TEXT_LEFT, "fps: %f", m_recording_ghost.m_fps);

            nk_layout_row_dynamic(m_ctx, 25, 1);
            nk_edit_string_zero_terminated(m_ctx,
                                           NK_EDIT_FIELD,
                                           m_ui_replay_path,
                                           sizeof(m_ui_replay_path),
                                           nk_filter_default);

            nk_layout_row_dynamic(m_ctx, 20, 1);
            nk_checkbox_label(m_ctx, "Overwrite", &m_ui_overwrite_file);
            nk_layout_row_dynamic(m_ctx, 25, 1);

            if (nk_button_label(m_ctx, "Save Replay")) {
                std::string path = m_ui_replay_path;

                if (path.size() == 0)
                    return;

                m_recording_ghost.save_to_file(path, m_ui_overwrite_file);
            }

            nk_tree_pop(m_ctx);
        }

        if (nk_tree_push(m_ctx, NK_TREE_TAB, "Other", NK_MINIMIZED)) {
            nk_layout_row_dynamic(m_ctx, 15, 1);
            nk_labelf(m_ctx, NK_TEXT_LEFT, "%.3f fps", 1.0 / dt);
            nk_labelf(m_ctx, NK_TEXT_LEFT, "%zu particles", m_ps.m_particles.size());
            nk_labelf(m_ctx, NK_TEXT_LEFT, "x: %f, y: %f", m_player.m_pos.x, m_player.m_pos.y);
            nk_tree_pop(m_ctx);
        }
    }
    nk_end(m_ctx);
}

void Game::begin_events()
{
    if (m_show_ui)
        nk_input_begin(m_ctx);
}

void Game::end_events()
{
    if (m_show_ui)
        nk_input_end(m_ctx);
}

void Game::load_texture(std::string path, SDL_Renderer* renderer, SDL_Texture** tex)
{
    auto fs = cmrc::assets::get_filesystem();
    cmrc::file file = fs.open(ASSETS_PATH + path);
    *tex = STBIMG_LoadTextureFromMemory(renderer, (const unsigned char*)file.begin(), file.size());

    if (tex == nullptr)
        error("failed to load texture \"" + path + "\": " + SDL_GetError());

    SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(*tex, SDL_ScaleModeLinear);
    SDL_Log("loaded texture \"%s\"", path.c_str());
}

void Game::load_textures(SDL_Renderer* renderer)
{
    load_texture("spike.png", renderer, &m_spike_texture);
}

void Game::process_event(SDL_Event* event)
{
    SDL_Scancode scancode;

    if (m_show_ui)
        nk_sdl_handle_event(event);

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
        case SDL_SCANCODE_U: // toggle ui
            m_show_ui = !m_show_ui;
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
    m_screen_w = screen_w;
    m_screen_h = screen_h;

    if (m_player.m_time_since_dead > 0.4) {
        reset();
        return;
    }

    m_player.collide(&m_level, m_camera.m_tile_top, m_camera.m_tile_bottom, &m_ps);
    Vec2 camera_target(m_player.m_pos.x + m_player.m_size.x / 2,
                       m_player.m_pos.y + m_player.m_size.y / 2);
    m_camera.update(camera_target, screen_w, screen_h);

    // the positions of tiles that are visible on the screen (from top-left to bottom-right)
    m_camera.m_tile_top.x = utils::clamp<int>(m_camera.m_pos.x / utils::tile_size, 0, m_level.m_width);
    m_camera.m_tile_top.y = utils::clamp<int>(m_camera.m_pos.y / utils::tile_size, 0, m_level.m_height);
    m_camera.m_tile_bottom.x = utils::clamp<int>((m_camera.m_pos.x + screen_w) / utils::tile_size + 1, 0, m_level.m_width);
    m_camera.m_tile_bottom.y = utils::clamp<int>((m_camera.m_pos.y + screen_h) / utils::tile_size + 1, 0, m_level.m_height);

    if (!m_player.m_dead && m_player.m_pos.y / utils::tile_size > m_level.m_height + 8) {
        m_player.die(&m_ps);
        return;
    }

    // record action
    if (m_player.m_did_move)
        m_recording_ghost.maybe_add_action(m_time, m_player.m_pos, m_player.m_grounded);
}

void Game::draw_background(SDL_Renderer* renderer)
{
    // TODO
}

void Game::render(SDL_Renderer* renderer, double dt)
{
    SDL_SetRenderDrawColor(renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, m_bg_color.a);
    SDL_RenderClear(renderer);

    auto draw_spike = [&](int x, int y) {
        if (m_spike_texture == nullptr)
            return;

        SDL_Rect dst_rect = m_level.tile_to_rect(x, y);
        m_camera.translate(&dst_rect);

        double angle = 0.0;

        if (m_level.tile_at(x, y+1)->m_type == SOLID)
            angle = 0.0;
        else if (m_level.tile_at(x, y-1)->m_type == SOLID)
            angle = 180.0;
        else if (m_level.tile_at(x-1, y)->m_type == SOLID)
            angle = 90.0;
        else if (m_level.tile_at(x+1, y)->m_type == SOLID)
            angle = -90.0;

        SDL_RenderCopyEx(
                    renderer,
                    m_spike_texture,
                    nullptr,
                    &dst_rect,
                    angle,
                    nullptr,
                    SDL_FLIP_NONE);
    };

    draw_background(renderer);

    // draw level
    for (int x = m_camera.m_tile_top.x; x < m_camera.m_tile_bottom.x; x++) {
        for (int y = m_camera.m_tile_top.y; y < m_camera.m_tile_bottom.y; y++) {
            Tile* tile = m_level.tile_at(x, y);

            switch (tile->m_type) {
            case SOLID:
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_Rect rect = m_level.tile_to_rect(x, y);
                m_camera.translate(&rect);
                SDL_RenderFillRect(renderer, &rect);
                break;
            }
            case HAZARD:
                draw_spike(x, y);
                break;
            default:
                continue;
            }
        }
    }

    // draw ghost
    if (m_player.m_did_move)
        m_playback_ghost.draw(renderer, &m_camera, m_time - m_player.m_move_start_time);

    // draw particles
    m_ps.draw(dt, &m_camera, renderer);

    // draw player
    m_player.draw(renderer, &m_camera);

    // draw ui
    if (m_show_ui) {
        draw_ui(dt);
        nk_sdl_render(NK_ANTI_ALIASING_ON);
    }

    SDL_RenderPresent(renderer);
}

void Game::load_level()
{
    m_level = Level("level1.png");
    m_playback_ghost.load_from_file_e("level1.run");
    reset();
}

void Game::reset()
{
    utils::tile_size = 48;
    m_level_start_time = m_time;
    m_recording_ghost.clear_actions();

    m_player = Player(); // reset player
    m_player.m_pos = Vec2<double>(m_level.m_spawn_pos.x * utils::tile_size,
                                 (m_level.m_spawn_pos.y-1) * utils::tile_size);
}

Game::~Game()
{
    SDL_DestroyTexture(m_spike_texture);
}
