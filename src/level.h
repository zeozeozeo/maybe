#ifndef LEVEL_H
#define LEVEL_H

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(assets);
#include <string>
#include <SDL.h>
#include <vector>
#include "vec2.h"
#include "camera.h"

enum TileType
{
    AIR,      // nothing
    SOLID,    // solid block
    HAZARD,   // spike
    PLATFORM, // moving platform
    SPAWN,    // spawn position
};

class Tile
{
public:
    Tile() = default;
    TileType m_type = AIR;
};

class Platform
{
public:
    Platform() = default;
    void update(void* level, double dt);
    void draw(SDL_Renderer* renderer, Camera* camera);
    SDL_Rect get_rect();
    void reset_pos() { m_pos = m_initial_pos; }

    Vec2<double> m_pos;
    Vec2<double> m_size;
    Vec2<double> m_initial_pos;
    double m_vel = -300.0; // x velocity
    Uint32 m_index;
};

class Level
{
public:
    Level(std::string path);
    Level() { }
    Tile* tile_at(int x, int y);
    SDL_Rect tile_to_rect(int x, int y);
    bool resolve_collision(Vec2<double>* pos, Vec2<double>* size, Vec2<int>tile_top, Vec2<int>tile_bottom, bool* is_hazard = nullptr, Uint32* ignore_platform = nullptr);

    std::vector<std::vector<Tile>> m_tiles; // [x][y]
    int m_width = 0, m_height = 0;
    Vec2<int> m_spawn_pos;
    std::vector<Platform> m_platforms;
private:
    Tile m_empty_tile;
};

#endif // LEVEL_H
