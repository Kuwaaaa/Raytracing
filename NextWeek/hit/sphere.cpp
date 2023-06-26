#include "sphere.h"
#include "../ONB.h"
#include "../pdf.h"

sphere::sphere(point3 cen, double r, shared_ptr<material> m)
    : center(cen), radius(r), mat_ptr(m) 
{ 

}

void sphere::get_sphere_uv(const point3& p, double& u, double& v)
{
    // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    auto theta = acos(-p.y());
    auto phi = atan2(-p.z(), p.x()) + pi;

    u = phi / (2 * pi);
    v = theta / pi;
}

// 判断ray与指定球是否有交点
// 如果有，填充rec为最近交点和球体圆心与交点组成的向量(即法向量), 并返回true
// 没有就返回false.
// 在最后强制将法向量转为从球体内部朝外的方向.
//
// 这里与球求交的公式是优化过后的, 通过将b缩放为half_b,计算整体减少了三次乘法
bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
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

const vec3 sphere::random(const point3& origin) const
{
    vec3 direction = center - origin;
    float distance_squared = direction.length_squared();
    Onb uvw;
    uvw.build_from_w(direction);
    
    return uvw.local(random_to_sphere(radius, distance_squared));
}


// ------------------------------------- MOVING SPHERE ------------------------------------------


moving_sphere::moving_sphere(point3 cen0, point3 cen1, double _time0, double _time1, double r, shared_ptr<material> m)
    : center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r), mat_ptr(m)
{

}

point3 moving_sphere::center(double time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

// Center become center(r.time()) from sphere
bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    auto outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool moving_sphere::bounding_box(double time0, double time1, aabb& output_box) const {
    aabb box0(
        center(time0) - vec3(radius),
        center(time0) + vec3(radius)
    );
    aabb box1(
        center(time1) - vec3(radius),
        center(time1) + vec3(radius)
    );
    output_box = surrounding_box(box0, box1);
    return true;
}

const vec3 random_to_sphere(float radius, float distance_squared)
{
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(1 - z * z);
    auto y = sin(phi) * sqrt(1 - z * z);

    return vec3(x, y, z);
}
