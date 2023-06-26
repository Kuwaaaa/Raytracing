#ifndef HITTABLE_H
#define HITTABLE_H

#include "../rtweekend.h"
#include "../vec3/vec3.h"
#include "../vec3/ray.h"
#include "aabb.h"
#include "../Entity.h"
#include "string"

class material;

struct hit_record 
{
    point3 p{ 0 };
    vec3 normal{ 0 };
    shared_ptr<material> mat_ptr{nullptr};
    double t{ 0 };
    double t2{ 0 };
    double u{ 0 };
    double v{ 0 };
    bool front_face{ false };
    std::string_view hittedObject;

    void set_face_normal(const ray& r, const vec3& outward_normal);
};

class hittable 
{
protected:
    Entity entity;
public:
    hittable() = default;
    hittable(const std::string_view& id) { setEntity(id); }
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;
    virtual const double pdf_value(const point3& o, const vec3& v) const;
    virtual const vec3 random(const vec3& o) const;
    virtual void setEntity(const std::string_view&);
};


class translate : public hittable {
public:
    translate(shared_ptr<hittable> p, const vec3& displacement);
    virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
    
public:
    shared_ptr<hittable> ptr;
    vec3 offset;
};


class rotate_y : public hittable {
    public:
        rotate_y(shared_ptr<hittable> p, double angle);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

    public:
        shared_ptr<hittable> ptr;
        double sin_theta;
        double cos_theta;
        bool hasbox;
        aabb bbox;
};



class flip_face : public hittable {
public:
    flip_face(shared_ptr<hittable> p) : ptr(p) {}

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override {

        if (!ptr->hit(r, t_min, t_max, rec))
            return false;

        rec.front_face = !rec.front_face;
        return true;
    }

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        return ptr->bounding_box(time0, time1, output_box);
    }

public:
    shared_ptr<hittable> ptr;
};


#endif // HITTABLE_H
