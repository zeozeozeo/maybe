#ifndef COLOR_H
#define COLOR_H

#include <SDL.h>

class Color
{
public:
    Color() = default;
    constexpr Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a):
        r(r), g(g), b(b), a(a)
    {}
    void apply(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }

    Uint8 r, g, b, a;
};

#endif // COLOR_H
