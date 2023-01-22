#ifndef MATERIAL_H
#define MATERIAL_H

#include "../rtweekend.h"

struct hit_record;

class material {
  public:
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const=0;
};

class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;

            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        color albedo;
};

class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            // 在反射射线上加入一个随机偏移，使反射射线的终点在一个以fuzz为半径的圆内随机偏移。
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            // 偏移后,当反射射线反射到了平面下时，返回false.理解为被吸收了.
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        color albedo;
        double fuzz;    // 反射射线随机偏移的圆的半径倍率
};

// 捏麻这个折射ray的计算看不懂啊啊啊啊
class dielectric : public material {
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) { }

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            attenuation = color(1.0, 1.0, 1.0);
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1);
            double sin_theta = sqrt(1 - cos_theta*cos_theta);
            
            // 折射率与sin的乘积 = 出射角的sin值,但出射角的sin值要保证小于等于1
            // 所以当折射率与sin的乘积大于1时,判定为没有折射,返回全反射的颜色.
            // 捏麻  看不出区别
            bool cannot_refract = refraction_ratio * sin_theta > 1;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);
            scattered = ray(rec.p, direction);
            return true;
        }

    public:
        double ir;  // Index of refraction

    private:
        // 这个应该就是那个菲涅尔的近似式了
        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx / (1+ref_idx));
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine), 5);
        }
};

#endif // MATERIAL_H