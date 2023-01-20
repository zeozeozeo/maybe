#ifndef VEC2_H
#define VEC2_H

template<typename T>
class Vec2
{
public:
    T x, y;
    Vec2(T x, T y):
        x(x), y(y)
    {};
    Vec2(T v):
        x(v), y(v)
    {};
    Vec2():
        x(0), y(0)
    {};
};

#endif // VEC2_H
