#include "player.h"

void Player::update(double dt, double time, ParticleSystem* ps, bool do_input)
{
    if (m_dead) {
        m_time_since_dead += dt;
        return;
    }
    m_time_since_dead = 0.0;

    if (do_input) {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        bool did_move_temp = m_did_move;
        m_walking = false;

        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {
            m_direction = LEFT;
            m_walking = true;
            m_did_move = true;
        }
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
            m_direction = RIGHT;
            m_walking = true;
            m_did_move = true;
        }
        // keep jumping when holdingd
        if ((keys[SDL_SCANCODE_SPACE] ||
                keys[SDL_SCANCODE_W] ||
                keys[SDL_SCANCODE_UP]) &&
                (m_grounded && m_jumps_since_landed == 0))
            jump();

        if (m_did_move && !did_move_temp) {
            m_move_start_time = time;
        }
    } else {
        m_walking = false;
    }

    // fall
    if (!m_grounded) {
        m_vel.y += P_FALL_ACCEL * dt;

        if (m_vel.y > P_MAX_FALL_SPEED)
            m_vel.y = P_MAX_FALL_SPEED;
    } else if (m_vel.y > 0.0) {
        // we are on the ground; reset the Y position
        m_vel.y = 0.0;
    }

    // walking
    if (!m_walking) {
        switch (m_direction) {
        case LEFT:
            if (m_vel.x < 0.0)
                m_vel.x += P_WALK_ACCEL * dt;
            if (m_vel.x > 0.0)
                m_vel.x = 0.0;
            break;
        case RIGHT:
            if (m_vel.x > 0.0)
                m_vel.x -= P_WALK_ACCEL * dt;
            if (m_vel.x < 0.0)
                m_vel.x = 0.0;
            break;
        }
    } else {
        m_vel.x = m_direction == LEFT ? -P_WALK_SPEED : P_WALK_SPEED;
        maybe_emit_walk_particles(ps, time);
    }

    double speed_mul = (double)utils::tile_size / 64.0;
    m_pos.x += (m_vel.x * speed_mul) * dt;
    m_pos.y += (m_vel.y * speed_mul) * dt;
}

void Player::collide(Level* level, Vec2<int> tile_top, Vec2<int> tile_bottom, ParticleSystem* ps)
{
    Vec2<int> top = { ((int)(m_pos.x)) / utils::tile_size-1, ((int)(m_pos.y)) / utils::tile_size-1 };
    Vec2<int> bottom = { ((int)(m_pos.x+m_size.x)) / utils::tile_size+1, ((int)(m_pos.y+m_size.y)) / utils::tile_size+1 };

    int prev_y = m_pos.y;
    bool is_hazard = false;
    bool colliding = level->resolve_collision(&m_pos, &m_size, top, bottom, &is_hazard);

    if (colliding && prev_y > m_pos.y) {
        m_grounded = true;
        m_jumps_since_landed = 0;
    } else {
        m_grounded = false;
    }

    if (is_hazard && !m_dead)
        die(ps);
}

void Player::jump()
{
    if (m_jumps_since_landed < 2) {
        m_vel.y = -P_JUMP_HEIGHT;
        m_jumps_since_landed++;
        m_did_move = true;
    }
}

SDL_Rect Player::get_rect()
{
    SDL_Rect rect = {
        (int)m_pos.x,
        (int)m_pos.y,
        (int)m_size.x,
        (int)m_size.y,
    };
    return rect;
}

void Player::maybe_emit_walk_particles(ParticleSystem* ps, double time)
{
    if (m_grounded && (time - m_last_particle_time > utils::rand_float(0.005, 0.01))) {
        Vec2 particle_size = Vec2<double>(utils::rand_float(8.0, 16.0));
        Vec2 particle_pos(
                    m_pos.x+m_size.x/2+utils::rand_float(-32.0, 16.0),
                    m_pos.y+m_size.y-particle_size.y/(64.0/(double)utils::tile_size));

        Particle* p = ps->add_particle(particle_pos,
                                       particle_size,
                                       {0, 0, 0, 255},
                                       0.5, 0.3);
        p->m_vel = Vec2(m_vel.x / 8.0 + utils::rand_float(-32.0, 32.0),
                        (double)utils::rand_float(0.0, -82.0));

        m_last_particle_time = time;
    }
}

void Player::emit_landing_particles(ParticleSystem* ps)
{
    // TODO
}

void Player::die(ParticleSystem *ps)
{
    if (m_dead)
        return;

    m_dead = true;
    int p_size = 16;
    float max_speed = 150.0;

    for (int x = m_pos.x; x < m_pos.x+m_size.x; x += p_size) {
        for (int y = m_pos.y; y < m_pos.y+m_size.y; y += p_size) {
            for (int i = 0; i < 2; i++) {
                Particle* p = ps->add_particle(
                            Vec2<double>(x, y),
                            Vec2<double>((float)p_size+utils::rand_float(-1.0, 1.0)),
                            Color(255, 0, 0, 255),
                            utils::rand_float(0.4, 0.7),
                            utils::rand_float(0.2, 0.3));

                p->m_vel.x = utils::rand_float(-max_speed, max_speed);
                p->m_vel.y = utils::rand_float(-max_speed, max_speed);
            }
        }
    }
}

void Player::draw(SDL_Renderer *renderer, Camera *camera)
{
    if (m_dead)
        return;

    SDL_Rect player_rect = get_rect();
    camera->translate(&player_rect);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player_rect);
}
