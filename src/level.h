#ifndef LEVEL_H
#define LEVEL_H

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(assets);
#include <string>
#include <SDL.h>
#include <vector>
#include "vec2.h"

#define ASSETS_FOLDER "assets/"
// #define TILE_SIZE 16

enum TileType
{
    AIR,
    SOLID,
    SPAWN,
};

class Tile
{
public:
    Tile() = default;
    TileType m_type = AIR;
};

class Level
{
public:
    Level(std::string path);
    Level() { }
    Tile* tile_at(int x, int y);
    SDL_Rect tile_to_rect(int x, int y);
    void resolve_collision(Vec2<double>* pos, Vec2<double>* size, Vec2<int>tile_top, Vec2<int>tile_bottom);

    std::vector<std::vector<Tile>> m_tiles; // [x][y]
    int m_width = 0, m_height = 0;
    Vec2<int> m_spawn_pos;
};

#endif // LEVEL_H
