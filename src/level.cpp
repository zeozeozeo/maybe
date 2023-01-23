#include "level.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "error.h"
#include "utils.h"

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
    // 0,255,0 (green): spawn position
    auto get_tile_type = [&](Uint8 r, Uint8 g, Uint8 b, Uint8 a) -> TileType {
        if (r == 255 && g == 255 && b == 255)
            return AIR;
        if (r == 0 && g == 0 && b == 0)
            return SOLID;
        if (r == 0 && g == 255 && b == 0)
            return SPAWN;

        SDL_Log("invalid block type (color %d %d %d %d) at %d %d, ignoring!", r, g, b, a, x, y);
        return AIR;
    };

    // allocate enough columns
    m_tiles.resize(width);

    for (x = 0; x < width; x++) {
        // allocate enough rows
        m_tiles[x].resize(height);

        for (y = 0; y < height; y++) {
            // for some reason clang-analyzer thinks that these are garbage values
            // so i'll just ignore it for now
#ifndef __clang_analyzer__
            const stbi_uc *p = data + (comp * (y * width + x));
            Uint8 r = p[0];
            Uint8 g = p[1];
            Uint8 b = p[2];
            Uint8 a = comp > 3 ? p[3] : 0xff;
#endif

            TileType type = get_tile_type(r, g, b, a);

            if (type == SPAWN) {
                m_spawn_pos = Vec2(x, y);
                SDL_Log("spawn position: %d %d", x, y);
            } else {
                tile_at(x, y)->m_type = type;
            }
        }
    }

    free(data);

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

bool Level::resolve_collision(Vec2<double>* pos, Vec2<double>* size, Vec2<int>tile_top, Vec2<int>tile_bottom)
{
    SDL_Rect rect;
    bool colliding = false;

    auto update_rect = [&]() {
        rect = { (int)pos->x, (int)pos->y, (int)size->x, (int)size->y };
    };
    update_rect();

    // tile_top.x = utils::clamp(tile_top.x, 0, m_width);
    // tile_top.y = utils::clamp(tile_top.y, 0, m_height);
    // tile_bottom.x = utils::clamp(tile_bottom.x, 0, m_width);
    // tile_bottom.y = utils::clamp(tile_bottom.y, 0, m_height);

    for (int x = tile_top.x; x < tile_bottom.x; x++) {
        for (int y = tile_top.y; y < tile_bottom.y; y++) {
            Tile* tile = tile_at(x, y);
            if (tile->m_type == AIR)
                continue; // don't collide with air

            SDL_Rect tile_rect = tile_to_rect(x, y);
            SDL_Rect r = { }; // result rect

            // try to intersect
            if (!SDL_IntersectRect(&tile_rect, &rect, &r))
                continue; // not intersecting; continue to the next tile

            colliding = true;

            // we have a collision, push the player out of the tile
            if (r.w < r.h) {
                // colliding on the x axis
                int delta = r.x-tile_rect.x;

                if (delta <= 0)
                    pos->x -= r.w; // push to the left
                else
                    pos->x += r.w; // push to the right
                update_rect();
            } else if (r.h < r.w) {
                // colliding on the y axis
                int delta = r.y-tile_rect.y;

                if (delta <= 0)
                    pos->y -= r.h; // push to the top
                else
                    pos->y += r.h; // push to the bottom
                update_rect();
            }
        }
    }

    return colliding;
}
