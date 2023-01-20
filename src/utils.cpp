#include "utils.h"
#include <cstdlib>
#include <time.h>

namespace utils
{
int tile_size;

void init()
{
    srand(time(nullptr));
}

float rand_float(float min, float max)
{
    // only 15 bits of the 23 floating point bits here are random,
    // but it doesn't really matter that much
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max-min)));
}

double lerp(double a, double b, double f)
{
    return a * (1.0 - f) + (b * f);
}
}
