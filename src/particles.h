#ifndef PARTICLES_H
#define PARTICLES_H

#include "color.h"
#include "vec2.h"
#include <vector>
#include <SDL.h>
#include "camera.h"

class Particle
{
public:
    Particle(Vec2<double> position, Vec2<double> size, Color color, double lifetime, double decay_time):
        m_pos(position),
        m_initial_size(size),
        m_color(color),
        m_lifetime(lifetime),
        m_decay_time(decay_time)
    {};
    Particle() = default;
    bool update(double dt); // returns whether it should be destroyed or not

    double m_life = 0.0;
    double m_lifetime = 1.0;
    double m_decay_time = 0.5; // the time when particles should start decaying
    Vec2<double> m_pos;
    Vec2<double> m_initial_size;
    Vec2<double> m_size;
    Vec2<double> m_vel;
    Color m_color;
};

class ParticleSystem
{
public:
    ParticleSystem() {};
    Particle* add_particle(Vec2<double> position, Vec2<double> size, Color color, double lifetime, double decay_time);
    void draw(double dt, Camera* camera, SDL_Renderer* renderer);

    std::vector<Particle> m_particles;
};

#endif // PARTICLES_H
