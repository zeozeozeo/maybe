#include "camera.h"
#include "utils.h"

void Camera::update(Vec2<double> move_target, int screen_w, int screen_h)
{
    Vec2 target(move_target.x - screen_w / 2, move_target.y - screen_h / 2);
    Vec2 lerp_pos(
                utils::lerp(m_pos.x, target.x, CAMERA_SPEED),
                utils::lerp(m_pos.y, target.y, CAMERA_SPEED));

    m_pos = lerp_pos;
}

void Camera::translate(SDL_Rect* rect)
{
    rect->x -= m_pos.x;
    rect->y -= m_pos.y;
}
