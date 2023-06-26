#ifndef BOX_H
#define BOX_H

#include "../rtweekend.h"
#include "../vec3/vec3.h"
#include "../vec3/ray.h"
#include "aarect.h"
#include "hittable_list.h"
#include "string"

class box : public hittable  {
public:
    box(const point3& p0, const point3& p1, shared_ptr<material> ptr, const std::string_view& id);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
    point3 box_min;
    point3 box_max;
    hittable_list sides;
};


#endif
