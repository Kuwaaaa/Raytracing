#ifndef RAYTRAING_H

#include "rtweekend.h"
#include "color.h"
#include "camera.h"
#include "hit/hittable_list.h"
#include "hit/sphere.h"
#include "material/material.h"
#include "hit/moving_sphere.h"
#include "hit/bvh.h"
#include "hit/texture.h"
#include "hit/aarect.h"
#include "hit/box.h"
#include "scene/scene.h"
#include "util/timer.h"


// Session2 : Add motion blur by add time property.
//            This is simulate shutter open and close.  
// Session3 : 实现了bvh. bvh通过对物体构建包围盒，来快速判定区域内是否有物体，极大加速了光追的速度。
//          这里实现的bvh将每个物体都建立一个包围框，形成叶节点。然后随机按某一个轴以大小将他们排序，用一个包围框将叶节点两两包围起来，
//          一层层包围起来直到最后所有物体都在一个包围框内。
//            包围框从最大的包围框开始，以判断ray与包围框的各个对称面的交点的前后顺序  
// Session4 : implemented texture.
// Session5 : perlin noise. reference to hit/perlin.h & hit/texture::noise_texture
// Session6 : Image texture. according to u,v and hitting point, get value from image. Import 3rd/stb_image.h
color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0, 0, 0);

    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);

    // vec3 unit_direction = unit_vector(r.direction());
    // auto t = 0.5*(unit_direction.y() + 1.0);
    // // Background color
    // return (1.0-t)*color(1, 1, 1) + t*color(0.2, 0.6, 0.6);
}

// Overload scene parameter
color ray_color(const ray& r, const Scene& scene) {
    return ray_color(r, scene.background, scene.world, scene.max_depth);
}


color PerPixel(size_t x, size_t y, const Scene& scene)
{
    color pixel_color(0, 0, 0);
    double u, v;
    for (int s = 0; s < scene.samples_per_pixel; ++s) {
        u = (x + random_double()) / (scene.image_width - 1);
        v = (y + random_double()) / (scene.image_height - 1);
        ray r = scene.cam.get_ray(u, v);
        pixel_color += ray_color(r, scene);
    }

    return pixel_color;
}

#endif // !RAYTRAING_H
