#include "particles.h"
#include "utils.h"

bool Particle::update(double dt)
{
    m_life += dt;
    m_pos.x += m_vel.x * dt;
    m_pos.y += m_vel.y * dt;

    double lerp_val = (m_life-m_decay_time) > 0.0 ?
              utils::lerp(0.0, m_decay_time, (m_life-m_decay_time)/(m_lifetime-m_decay_time)) / m_decay_time
              : 0.0;

    double size_mul = (double)utils::tile_size / 64.0;
    m_size.x = (m_initial_size.x - lerp_val * m_initial_size.x) * size_mul;
    m_size.y = (m_initial_size.y - lerp_val * m_initial_size.y) * size_mul;

    return m_life >= m_lifetime;
}

Particle* ParticleSystem::add_particle(Vec2<double> position, Vec2<double> size, Color color, double lifetime, double decay_time)
{
    Particle p(position, size, color, lifetime, decay_time);
    m_particles.push_back(p);
    return &m_particles.at(m_particles.size() - 1);
}

void ParticleSystem::draw(double dt, Camera* camera, SDL_Renderer* renderer)
{
    for (int i = 0; i < m_particles.size(); i++) {
        auto& p = m_particles.at(i);
        if (p.update(dt)) {
            // destroy the particle
            m_particles.erase(m_particles.begin() + i);
            i--;
            continue;
        }

        // draw the particle
        SDL_Rect rect = {
            (int)p.m_pos.x,
            (int)p.m_pos.y,
            (int)p.m_size.x,
            (int)p.m_size.y,
        };

        camera->translate(&rect);
        p.m_color.apply(renderer);
        SDL_RenderFillRect(renderer, &rect);
    }
}
