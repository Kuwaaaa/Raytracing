#ifndef CAMERA_H
#define CAMERA_H

#include "vec3/vec3.h"
#include "vec3/ray.h"

// 

class Camera {
    public:
        Camera(
            double aspect_ratio = 16 / 9,
            point3 lookfrom = point3(13, 2, 13),
            point3 lookat = point3(0, 0, 0),
            point3 vup = point3(0, 1, 0),
            double vfov = 40.0,        // vertical field-of-view in degrees
            double aperture = 0,
            double focus_dist = (point3(13, 2, 13) - point3(0, 0, 0)).length(),
            double _time0 = 0,
            double _time1 = 0
         ) {
            // 当theta==90时, h = 1,图跟以前一样
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            // auto aspect_ratio = 16.0 / 9.0;
            // auto viewport_height = 2.0;
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;
            auto focal_length = 1.0;
            
            // w为lookat指向camera的向量
            // u为右向量,通过w与上向量叉乘得到.
            // v为camera的上向量,与w垂直.
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup,w));
            v = cross(w, u);
            
            // 没有Camera时原点默认在(0,0,0), 成像平面平行于xy轴,模长分别为屏幕长宽.
            // origin = 0;
            // horizontal = vec3(viewport_width, 0.0, 0.0);
            // vertical = vec3(0.0, viewport_height, 0.0);

            // viewport_width * u 使得成像平面的水平轴指向u方向,模长为viewport_width.
            // 或者理解成uv为成像平面方向，然后模长*viewport_width,viewport_height.
            // focus_dist 还没参透QAQ
            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;
        }

        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            return ray(origin + offset,
             lower_left_corner + s*horizontal + t*vertical - origin - offset,
             random_double(time0, time1));
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
        double time0;
        double time1;
};

#endif // CAMERA_H
