#ifndef RAY_H
#define RAY_H

#include "vec3.h"
//#include "../rtweekend.h"

class ray {
public:
    ray() = default;
    ray(const point3& origin, const vec3& direction, double time = 0.0);

    // With const and &, it can be speed up a lot. Here test in random scene at expleam
    // debug: 8.9s, relese: 1.43s
    /*point3 origin() const { return orig; }
    vec3 direction() const { return dir; }*/

    // debug 7.89s, relese: 1.33s
    const point3& origin() const;
    const vec3& direction() const;
    double time() const;
    const point3& at(double t) const;

public:
    point3 orig;
    vec3 dir;
    double tm;
};

#endif // RAY_H
