#ifndef AARECT_H
#define AARECT_H

#include "../rtweekend.h"
#include "../vec3/vec3.h"
#include "../vec3/ray.h"
#include "hittable.h"
#include "string"


// TODO:
// 把这些整合成一个类

class xy_rect : public hittable {
    public:
        xy_rect() = default;
        // Make sure x0 < x1, y0 < y1 
        xy_rect(double _x0, double _x1, double _y0, double _y1, double _k,
            shared_ptr<material> mat, const std::string_view&);
            
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
        //virtual const vec3& random(const point3& origin) const override;

    public:
        shared_ptr<material> mp;
        double x0, x1, y0, y1, k;
};


class xz_rect : public hittable {
    public:
        xz_rect() = default;
        xz_rect(double _x0, double _x1, double _z0, double _z1, double _k,
            shared_ptr<material> mat, const std::string_view&);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
        virtual const double pdf_value(const point3& origin, const vec3& v) const override;
        virtual const vec3 random(const point3& origin) const override;

    public:
        shared_ptr<material> mp;
        double x0, x1, z0, z1, k;
};


class yz_rect : public hittable {
    public:
        yz_rect() = default;
        yz_rect(double _y0, double _y1, double _z0, double _z1, double _k,
            shared_ptr<material> mat, const std::string_view&);
            
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
        //virtual const vec3& random(const point3& origin) const override;

    public:
        shared_ptr<material> mp;
        double y0, y1, z0, z1, k;
};

#endif
