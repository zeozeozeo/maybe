#ifndef PLAYER_H
#define PLAYER_H

#include "level.h"
#include "particles.h"
#include "vec2.h"
#include <SDL.h>
#include "utils.h"

#define P_MAX_FALL_SPEED 5000.0
#define P_FALL_ACCEL 3500.0
#define P_WALK_SPEED 900.0
#define P_WALK_ACCEL 6000.0
#define P_JUMP_HEIGHT 1200.0

enum Direction
{
    LEFT,
    RIGHT,
};

class Player
{
public:
    Player() = default;
    void update(double dt, double time, ParticleSystem* ps);
    void collide(Level* level, Vec2<int> tile_top, Vec2<int> tile_bottom, ParticleSystem *ps);
    void jump();
    void maybe_emit_walk_particles(ParticleSystem* ps, double time);
    void emit_landing_particles(ParticleSystem* ps);
    SDL_Rect get_rect();

    Vec2<double> m_vel; // velocity
    Vec2<double> m_pos; // position
    Vec2<double> m_size = Vec2<double>(utils::tile_size, utils::tile_size * 2); // width & height
    bool m_grounded = false;
    Direction m_direction = RIGHT;
    bool m_walking = false;
    int m_jumps_since_landed = 0;
    double m_last_particle_time = 0.0;
};

#endif // PLAYER_H
