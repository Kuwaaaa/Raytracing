#ifndef MATERIAL_H
#define MATERIAL_H

#include "../rtweekend.h"
#include "vec3/vec3.h"
#include "vec3/ray.h"
#include "../hit/texture.h"
#include "../ONB.h"
#include "../pdf.h"
// Section2: return ray with its time

struct hit_record;

struct scatter_record
{
    ray specular_ray;
    bool is_specular;
    vec3 fr;
    shared_ptr<pdf> pdf_ptr;

    /*~scatter_record() {
        delete pdf_ptr;
    }*/
};

class material {
  public:
      virtual bool scatter(
          const ray& r_in, const hit_record& rec, scatter_record& srec
      ) const {
          return false;
    }

      virtual double scattering_pdf(
          const ray& r_in, const hit_record& rec, const ray& scattered
      ) const {
          return 0;
      }
    virtual color emitted(const ray& r_in, const hit_record& rec) const {
        return color(0, 0, 0);
    }
};

class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
        lambertian(shared_ptr<Texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            //auto scatter_direction = rec.normal + random_unit_vector();

            //// Catch degenerate scatter direction
            //if (scatter_direction.near_zero())
            //    scatter_direction = rec.normal;
            /*Onb uvw;
            uvw.build_from_w(rec.normal);
            auto direction = uvw.local(random_cosine_direction());
            scattered = ray(rec.p, unit_vector(direction), r_in.time());*/
            srec.fr = albedo->value(rec.u, rec.v, rec.p);
            srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
            return true;
        }
        double scattering_pdf(
            const ray& r_in, const hit_record& rec, const ray& scattered
        ) const {
            auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
            return cosine < 0 ? 0 : cosine / pi;
        }
    public:
        shared_ptr<Texture> albedo;
};

//class metal : public material {
//    public:
//        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
//
//        virtual bool scatter(
//            const ray& r_in, const hit_record& rec, scatter_record& srec
//        ) const override {
//            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
//            // 在反射射线上加入一个随机偏移，使反射射线的终点在一个以fuzz为半径的圆内随机偏移。
//            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
//            attenuation = albedo;
//            // 偏移后,当反射射线反射到了平面下时，返回false.理解为被吸收了.
//            return (dot(scattered.direction(), rec.normal) > 0);
//        }
//
//    public:
//        color albedo;
//        double fuzz;    // 反射射线随机偏移的圆的半径倍率
//};
//
//// 捏麻这个折射ray的计算看不懂啊啊啊啊
//class dielectric : public material {
//    public:
//        dielectric(double index_of_refraction) : ir(index_of_refraction) { }
//
//        virtual bool scatter(
//            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf
//        ) const override {
//            attenuation = color(1.0, 1.0, 1.0);
//            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;
//
//            vec3 unit_direction = unit_vector(r_in.direction());
//            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1);
//            double sin_theta = sqrt(1 - cos_theta*cos_theta);
//            
//            // 折射率与sin的乘积 = 出射角的sin值,但出射角的sin值要保证小于等于1
//            // 所以当折射率与sin的乘积大于1时,判定为没有折射,返回全反射的颜色.
//            // 捏麻  看不出区别
//            bool cannot_refract = refraction_ratio * sin_theta > 1;
//            vec3 direction;
//
//            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
//                direction = reflect(unit_direction, rec.normal);
//            else
//                direction = refract(unit_direction, rec.normal, refraction_ratio);
//            scattered = ray(rec.p, direction, r_in.time());
//            return true;
//        }
//
//    public:
//        double ir;  // Index of refraction
//
//    private:
//        // 这个应该就是那个菲涅尔的近似式了
//        static double reflectance(double cosine, double ref_idx) {
//            // Use Schlick's approximation for reflectance.
//            auto r0 = (1-ref_idx / (1+ref_idx));
//            r0 = r0*r0;
//            return r0 + (1-r0)*pow((1 - cosine), 5);
//        }
//};
//
class diffuse_light : public material  {
    public:
        diffuse_light(shared_ptr<Texture> a) : emit(a) {}
        diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            return false;
        }

        virtual color emitted(const ray& r_in, const hit_record& rec) const override {
            //if (rec.front_face)
            if (1)
                return emit->value(rec.u, rec.v, rec.p);
            else
                return color(0, 0, 0);
        }

    public:
        shared_ptr<Texture> emit;
};
//
//class volume : public material {
//public:
//    volume() : sigma_a(0.1) { }
//    volume(color c, float a = 0.1) : vColor(c), sigma_a(a) { }
//
//    virtual bool scatter(
//            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf
//        ) const override {
//            /*scattered.origin() = rec.t2;
//            scattered.direction() = r_in.direction();
//
//            attenuation = (1 - sigma_a) * vColor;*/
//            return true;
//        }
//public:
//    color vColor;
//    double sigma_a;
//};

#endif // MATERIAL_H
