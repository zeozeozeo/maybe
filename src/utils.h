#ifndef UTILS_H
#define UTILS_H

#define ASSETS_PATH "assets/"

namespace utils
{
extern int tile_size;
void init();
float rand_float(float min, float max);
double lerp(double a, double b, double f);

template<typename T>
T clamp(T val, T min, T max) {
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

template<typename T>
T clamp_min(T val, T min) {
    if (val < min)
        return min;
    return val;
}
}

#endif // UTILS_H
