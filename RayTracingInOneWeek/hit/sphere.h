#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "../vec3/vec3.h"

class sphere : public hittable {
    public:
        sphere() { }
        sphere(point3 cen, double r, shared_ptr<material> m)
         : center(cen), radius(r), mat_ptr(m) { }

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

// 判断ray与指定球是否有交点
// 如果有，填充rec为最近交点和球体圆心与交点组成的向量(即法向量), 并返回true
// 没有就返回false.
// 在最后强制将法向量转为从球体内部朝外的方向.
//
// 这里与球求交的公式是优化过后的, 通过将b缩放为half_b,计算整体减少了三次乘法
bool sphere:: hit (const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    // 从近到远判断两个根的值是否在t_min、t_max之间.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
        return false;    
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_norm = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_norm);
    rec.mat_ptr = mat_ptr;
    // rec.normal = outward_norm;

    return true;
}

#endif // SPHERE_H