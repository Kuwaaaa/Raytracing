#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
public:
    sphere() = default;
    sphere(point3 cen, double r, shared_ptr<material> m);

    static void get_sphere_uv(const point3& p, double& u, double& v);
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};

// Add time property based sphere
class moving_sphere : public hittable {
public:
    moving_sphere() = default;
    moving_sphere(point3 cen0, point3 cen1, double _time0, double _time1, double r, shared_ptr<material> m);

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

    point3 center(double time) const;

public:
    point3 center0, center1;
    double time0 = 0, time1 = 0;
    double radius = 1;
    shared_ptr<material> mat_ptr;
};


#endif // SPHERE_H
