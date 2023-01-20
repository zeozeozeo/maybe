#ifndef CAMERA_H
#define CAMERA_H

#include "vec2.h"
#include <SDL.h>

#define CAMERA_SPEED 0.1

class Camera
{
public:
    Camera() = default;
    void update(Vec2<double> move_target, int screen_w, int screen_h);
    void translate(SDL_Rect* rect);

    Vec2<double> m_pos;
};

#endif // CAMERA_H
