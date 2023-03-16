#ifndef AABB_H
#define AABB_H

#include "../rtweekend.h"
#include "../vec3/vec3.h"
#include "../vec3/ray.h"

class aabb {
public:
    aabb() = default;
    aabb(const point3&a, const point3&b )
        : minimum(a), maximum(b) { }

    // debug: 7541ms; relese:1380ms
    /*point3 min() const {return minimum; }
    point3 max() const {return maximum; }*/

    // debug: 6004ms; relese:1283ms
    const point3& min() const;
    const point3& max() const;

    bool hit(const ray& r, double t_min, double t_max) const;

public:
    // 这两个点表示包围盒的两个对角点
    point3 minimum;
    point3 maximum;
};

aabb surrounding_box(aabb box0, aabb box1);

#endif
