#include "ray.h"

ray::ray(const point3& origin, const vec3& direction, double time)
    : orig(origin), dir(direction), tm(time)
{ }

const point3& ray::origin() const
{
    return orig;
}

const vec3& ray::direction() const
{
    return dir;
}

double ray::time() const
{
    return tm;
}

const point3& ray::at(double t) const
{
    return orig + t * dir;
}
