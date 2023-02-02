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

#include "jpeglib.h"

#include "util/timer.h"

#include <future>
#include <thread>

#include <iostream>

using namespace std;
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
        return color(0,0,0);

    if (!world.hit(r, 0.001, infinity, rec))
        return background;
    
    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if(!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
    
    // vec3 unit_direction = unit_vector(r.direction());
    // auto t = 0.5*(unit_direction.y() + 1.0);
    // // Background color
    // return (1.0-t)*color(1, 1, 1) + t*color(0.2, 0.6, 0.6);
}

hittable_list random_scene() {
    hittable_list world;

    // Session4 add
    // ------------
    // auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    // // world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));
    // auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    // world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));
    // ------------

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;

}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto perlinTex = make_shared<noise_texture>();

    objects.add(make_shared<sphere>(point3(0,-1000, 0), 1000, make_shared<lambertian>(perlinTex)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(perlinTex)));

    return objects;

}

hittable_list earth() {
    auto earth_texture = make_shared<img_texture>("images/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 3.0f, earth_surface);

    return hittable_list(globe);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>();
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    // objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    objects.add(make_shared<xy_rect>(10, 5, 1, 10, -2, make_shared<lambertian>(color(0.5))));
    // objects.add(make_shared<sphere>(point3(0,5,0), 2, difflight));


    return objects;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);
    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);

    return objects;
}

hittable_list volumeTest() {
    hittable_list objects;

    objects.add(make_shared<sphere>(point3(0, 0, 0), 3.0f, make_shared<volume>(color(0.8f, 0.1f, 0.1f))));
    objects.add(make_shared<xy_rect>(0, 1000, 0, 1000, 10, make_shared<lambertian>(color(0.5))));
    return hittable_list(objects);
}

void renderPass(const Scene& scene, color& pixel_color, double u, double v, int& count) {
    ray r = scene.cam.get_ray(u, v);
    pixel_color += ray_color(r, scene.background, scene.world, scene.max_depth);
    count --;
}

int main()
{
    // Image
    auto aspect_radio = 16.0/9.0;
    int image_width = 400;
    int samples_per_pixel = 50;
    int max_depth = 10;

    // World
    auto R = cos(pi/4);
    // hittable_list world = random_scene();
    // bvh_node bvh(world, 0, 1);

    // Camera
    // 发现这里相机在物体里面的话渲染会变得很慢
    // 理解为每次求交都成功了
    // 通过调整lookfrom发现较近处的物体虚化比较大
    point3 lookfrom(13, 2, 13);
    point3 lookat(0, 0, 0);
    point3 vup(0, 1, 0);
    float vfov = 40.0f;
    auto dist_to_focus = (lookfrom - lookat).length();
    // auto dist_to_focus = sqrt(13);
    float aperture = 0;
    color background(0.9, 0.9, 0.9);
    
    bvh_node world;
    switch (1)
    {
    case 1:
        world = random_scene();
        aperture = 0.1;
        break;

    default:
    case 2:
        world = two_spheres();
        break;

    case 3:
        world = two_perlin_spheres();
        break;

    case 4:
        vfov = 20;
        world = earth();
        break;

    case 5:
            world = simple_light();
            samples_per_pixel = 400;
            lookfrom = point3(26,3,6);
            lookat = point3(0,2,0);
            vfov = 20.0;
            break;

    case 6:
            world = cornell_box();
            image_width = 640;
            aspect_radio = 1.0;
            samples_per_pixel = 400;
            background = color(0,0,0);
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
            
    case 7:
            world = volumeTest();
            break;
    }
    int image_height = static_cast<int>(image_width / aspect_radio);

    // camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 20.0, aspect_radio);
    camera cam(lookfrom, lookat, vup, vfov, aspect_radio, aperture, dist_to_focus, 0.0, 1.0);

    Scene scene(world, cam, samples_per_pixel, background, image_width, aspect_radio,
            max_depth);

    
    // Render
    std::cout <<  "P3\n" << image_width << " " << image_height << "\n255\n";
    double u, v;
    Timer timer(&cerr);
    for (int j = image_height-1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s){
                u = (i + random_double()) / (image_width-1);
                v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
                
            } 
            write_color(std::cout, pixel_color, samples_per_pixel);
        }   
    }

    std::cerr << "\nDone.\n";
}