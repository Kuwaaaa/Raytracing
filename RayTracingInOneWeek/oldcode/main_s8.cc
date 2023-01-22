#include "rtweekend.h"

#include "color.h"
#include "hit/hittable_list.h"
#include "hit/sphere.h"
#include "camera.h"

#include <iostream>

// 
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        // 漫反射
        // point3 target = rec.p + rec.normal + random_in_unit_sphere();
        // 球上点转到球面上点0.0  实际上就是把原点到球上随机点的向量转为单位向量，这个向量就变成了到球面上向量
        // 地上的阴影最少，总体感觉亮一点
        // point3 target = rec.p + rec.normal + random_unit_vector();
        // 平面外反射
        // 这玩意的球在边缘的阴影会更多点，看起来更立体点。
        // 相对来说地面上的阴影最多
        point3 target = rec.p + rec.normal + random_in_hemisphere(rec.normal);
        return 0.3 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main()
{
    // Image
    const auto aspect_radio = 16.0/9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_radio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    hittable_list world;
    // 向world中添加两个球体
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    camera cam;

    // Render

    std::cout <<  "P3\n" << image_width << " " << image_height << "\n255\n";
    double u, v;
    for (int j = image_height-1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s){
                u = (i + random_double()) / (image_width-1);
                v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            } 
            write_color(std::cout, pixel_color, samples_per_pixel);
        }   
    }

    std::cerr << "\nDone.\n";
}