#include "level.h"
// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "error.h"
#include "utils.h"

SDL_Rect Platform::get_rect()
{
    return { (int)m_pos.x, (int)m_pos.y, (int)m_size.x, (int)m_size.y };
}

void Platform::update(void* l, double dt)
{
    Level* level = (Level*)l;

    Vec2<double> prev_pos = m_pos;

    Vec2<int> top = { ((int)(m_pos.x)) / utils::tile_size-1, ((int)(m_pos.y)) / utils::tile_size-1 };
    Vec2<int> bottom = { ((int)(m_pos.x+m_size.x)) / utils::tile_size+1, ((int)(m_pos.y+m_size.y)) / utils::tile_size+1 };

    level->resolve_collision(&m_pos, &m_size, top, bottom, nullptr, &m_index);

    if (m_pos.x != prev_pos.x)
        m_vel = -m_vel;

    m_pos.x += m_vel * dt;
}

void Platform::draw(SDL_Renderer* renderer, Camera* camera)
{
    SDL_Rect rect = get_rect();
    camera->translate(&rect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

Level::Level(std::string path)
{
    auto fs = cmrc::assets::get_filesystem();
    auto image_file = fs.open(ASSETS_PATH + path);
    const stbi_uc* buffer = (stbi_uc*)image_file.cbegin();

    int width = 0, height = 0, comp = 0;
    stbi_uc* data = stbi_load_from_memory(buffer, image_file.size(), &width, &height, &comp, false);

    SDL_Log("loading level %s (width: %d, height: %d, comp: %d)", path.c_str(), width, height, comp);

    if (data == nullptr)
        error("failed to decode level image!");

    int x = 0, y = 0;

    // 255,255,255 (white): air
    // 0,0,0 (black): solid
    // 255,0,0 (red): hazard (spike)
    // 0,0,255 (blue): moving platform
    // 0,255,0 (green): spawn position
    auto get_tile_type = [&](Uint8 r, Uint8 g, Uint8 b, Uint8 a) -> TileType {
        if (r == 255 && g == 255 && b == 255)
            return AIR;
        if (r == 0 && g == 0 && b == 0)
            return SOLID;
        if (r == 255 && g == 0 && b == 0)
            return HAZARD;
        if (r == 0 && g == 0 && b == 255)
            return PLATFORM;
        if (r == 0 && g == 255 && b == 0)
            return SPAWN;

        SDL_Log("invalid block type (color %d %d %d %d) at %d %d, ignoring!", r, g, b, a, x, y);
        return AIR;
    };

    m_platforms.clear();

    auto handle_platform = [&](int x, int y, Uint32 idx) {
        // TODO: currently platforms can only be one tile in height
        int start_x = x;
        for (;; x++) {
            Tile* tile = tile_at(x, y);
            if (tile->m_type == PLATFORM)
                tile->m_type = AIR;
            else
                break;
        }

        int platform_width = x - start_x;

        if (platform_width <= 0)
            return;

        Platform platform;

        platform.m_pos = Vec2<double>(x * utils::tile_size, y * utils::tile_size);
        platform.m_size = Vec2<double>(platform_width * utils::tile_size, utils::tile_size);
        platform.m_index = idx;
        platform.m_initial_pos = platform.m_pos;

        m_platforms.push_back(platform);
    };

    std::vector<Vec2<int>> platforms;

    // allocate enough columns
    m_tiles.resize(width);

    for (x = 0; x < width; x++) {
        // allocate enough rows
        m_tiles[x].resize(height);

        for (y = 0; y < height; y++) {
            const stbi_uc *p = data + (comp * (y * width + x));
            Uint8 r = p[0];
            Uint8 g = p[1];
            Uint8 b = p[2];
            Uint8 a = comp > 3 ? p[3] : 0xff;

            TileType type = get_tile_type(r, g, b, a);

            switch (type) {
            case SPAWN:
                m_spawn_pos = Vec2(x, y);
                SDL_Log("found spawn position: %d %d", x, y);
                break;
            case PLATFORM:
                platforms.push_back({ x, y });
                // no break here, this is important
            default:
                tile_at(x, y)->m_type = type;
                break;
            }
        }
    }

    free(data);

    Uint32 idx = 0;
    for (auto& platform : platforms) {
        handle_platform(platform.x, platform.y, idx);
        idx++;
    }

    m_width = width;
    m_height = height;
}

Tile* Level::tile_at(int x, int y)
{
    if (x < 0 || y < 0 || x >= m_tiles.size() || y >= m_tiles[x].size()) {
        // maybe someone tried to overwrite it
        if (m_empty_tile.m_type != AIR)
            m_empty_tile = Tile();
        return &m_empty_tile;
    }
    return &m_tiles[x][y];
}


SDL_Rect Level::tile_to_rect(int x, int y)
{
    return { x * utils::tile_size, y * utils::tile_size, utils::tile_size, utils::tile_size };
}

bool Level::resolve_collision(Vec2<double>* pos, Vec2<double>* size, Vec2<int>tile_top, Vec2<int>tile_bottom, bool* is_hazard, Uint32* ignore_platform)
{
    SDL_Rect rect;
    bool colliding = false;

    auto update_rect = [&]() {
        rect = { (int)pos->x, (int)pos->y, (int)size->x, (int)size->y };
    };
    update_rect();

    auto collide_rect = [&](SDL_Rect* coll_rect) -> bool {
        SDL_Rect r = { }; // result rect

        // try to intersect
        if (!SDL_IntersectRect(coll_rect, &rect, &r))
            return false;

        // we have a collision, push the player out of the tile
        if (r.w < r.h) {
            // colliding on the x axis
            int delta = r.x-coll_rect->x;

            if (delta <= 0)
                pos->x -= r.w; // push to the left
            else
                pos->x += r.w; // push to the right
            update_rect();
        } else if (r.h < r.w) {
            // colliding on the y axis
            int delta = r.y-coll_rect->y;

            if (delta <= 0)
                pos->y -= r.h; // push to the top
            else
                pos->y += r.h; // push to the bottom
            update_rect();
        }
        return true;
    };

    // collide with tiles
    for (int x = tile_top.x; x < tile_bottom.x; x++) {
        for (int y = tile_top.y; y < tile_bottom.y; y++) {
            Tile* tile = tile_at(x, y);

            if (tile->m_type == AIR)
                continue; // don't collide with air

            SDL_Rect tile_rect = tile_to_rect(x, y);

            // the if condition here is important, we do not want to
            // overwrite the previous value since collide_rect can return false
            if (collide_rect(&tile_rect))
                colliding = true;

            if (colliding && is_hazard != nullptr && tile->m_type == HAZARD)
                *is_hazard = true;
        }
    }

    // collide with platforms
    for (auto& platform : m_platforms) {
        if (ignore_platform != nullptr && *ignore_platform == platform.m_index)
            continue;

        SDL_Rect platform_rect = platform.get_rect();

        if (collide_rect(&platform_rect))
            colliding = true;
    }

    return colliding;
}
