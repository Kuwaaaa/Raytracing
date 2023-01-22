#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
    public:
        camera(
            point3 lookfrom,
            point3 lookat,
            point3 vup,
            double vfov,        // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist
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
            // 折磨
            // std::cerr << "camera lower_corner" << lower_left_corner << std::endl;
            // std::cerr << "camera viewport_width " << viewport_width << std::endl;
            // std::cerr << "camera viewport_height " << viewport_height << std::endl;
            // std::cerr << "camera vertical " << vertical/2 << std::endl;

            // float fov = 2.0f * atan((sensorWidth / (2.0f * focal_length))); // in radians
            // float tan_fov = tanf(fov / 2.0f);
            
            // // create point on lens, screen space samples are in [-1, 1] domain
            // vec3 p(screen_space_sample_x * tan_fov, screen_space_sample_y * tan_fov, 1.0);
            
            // // calculate direction vector from origin to point on lens
            // dir = normalize(p - origin);
            
            // // get uniformly distributed points on the unit disk
            // vec2 lens(0.0, 0.0);
            // concentricDiskSample(random1, random2, &lens);
            
            // // scale points in [-1, 1] domain to actual aperture radius
            // lens *= apertureRadius;
            
            // // new origin is these points on the lens
            // origin.x = lens.x;
            // origin.y = lens.y;
            // origin.z = 0.0;
            
            // // Compute point on plane of focus, intersection on z axis
            // float intersection = std::abs(focus_distance / dir.z);
            // vec3 focusPoint = dir * intersection;
            // dir = normalize(focusPoint - origin);
            
            // // now looking down -Z (optional depending on your coordinate system)
            // dir.z *= -1.0;
        }

        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            return ray(origin + offset,
             lower_left_corner + s*horizontal + t*vertical - origin - offset);
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
};

#endif // CAMERA_H