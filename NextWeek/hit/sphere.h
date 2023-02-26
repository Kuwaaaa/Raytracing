#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3/vec3.h"

class sphere : public hittable {
    public:
        sphere() { }
        sphere(point3 cen, double r, shared_ptr<material> m)
         : center(cen), radius(r), mat_ptr(m) { }

        static void get_sphere_uv(const point3& p, double& u, double& v) {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

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
    auto root0 = (-half_b - sqrtd) / a;
    auto root1 = (-half_b + sqrtd) / a;
    if (root0 < t_min || t_max < root0) {
        std::swap(root0, root1);
        if (root0 < t_min || t_max < root0)
        return false;    
    }

    rec.t = root0;
    rec.t2 = root1;
    rec.p = r.at(rec.t);
    vec3 outward_norm = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_norm);
    get_sphere_uv(outward_norm, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;
    // rec.normal = outward_norm;

    return true;
}

bool sphere::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = aabb(
        center - vec3(radius),
        center + vec3(radius)
    );
    return true;
}

#endif // SPHERE_H
