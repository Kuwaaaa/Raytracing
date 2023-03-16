#ifndef RAY_EXPLEAM_H
#define RAY_EXPLEAM_H

#include "rtweekend.h"
#include "vec3/vec3.h"
#include "vec3/ray.h"
#include "color.h"
#include "camera.h"
#include "hit/hittable_list.h"
#include "hit/sphere.h"
#include "material/material.h"
#include "hit/bvh.h"
#include "hit/texture.h"
#include "hit/aarect.h"
#include "hit/box.h"
#include "scene/scene.h"

//hittable_list random_scene() {
//	hittable_list world;
//
//	// Session4 add
//	// ------------
//	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
//	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
//	// auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
//	// world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));
//	// ------------
//
//	for (int a = -11; a < 11; a++) {
//		for (int b = -11; b < 11; b++) {
//			auto choose_mat = random_double();
//			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
//
//			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
//				shared_ptr<material> sphere_material;
//
//				if (choose_mat < 0.8) {
//					// diffuse
//					auto albedo = color::random() * color::random();
//					sphere_material = make_shared<lambertian>(albedo);
//					auto center2 = center + vec3(0, random_double(0, .5), 0);
//					world.add(make_shared<moving_sphere>(
//						center, center2, 0.0, 1.0, 0.2, sphere_material));
//				}
//				else if (choose_mat < 0.95) {
//					// metal
//					auto albedo = color::random(0.5, 1);
//					auto fuzz = random_double(0, 0.5);
//					sphere_material = make_shared<metal>(albedo, fuzz);
//					world.add(make_shared<sphere>(center, 0.2, sphere_material));
//				}
//				else {
//					// glass
//					sphere_material = make_shared<dielectric>(1.5);
//					world.add(make_shared<sphere>(center, 0.2, sphere_material));
//				}
//			}
//		}
//	}
//
//	auto material1 = make_shared<dielectric>(1.5);
//	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
//
//	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
//	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));
//
//	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
//	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
//
//	return world;
//}
//
//hittable_list two_spheres() {
//	hittable_list objects;
//
//	auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
//
//	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
//	objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));
//
//	return objects;
//
//}
//
//hittable_list two_perlin_spheres() {
//	hittable_list objects;
//
//	auto perlinTex = make_shared<noise_texture>();
//
//	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(perlinTex)));
//	objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(perlinTex)));
//
//	return objects;
//
//}
//
//hittable_list earth() {
//	auto earth_texture = make_shared<img_texture>("images/earthmap.jpg");
//	auto earth_surface = make_shared<lambertian>(earth_texture);
//	auto globe = make_shared<sphere>(point3(0, 0, 0), 3.0f, earth_surface);
//
//	return hittable_list(globe);
//}
//
//hittable_list simple_light() {
//	hittable_list objects;
//
//	auto pertext = make_shared<noise_texture>();
//	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
//	// objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));
//
//	auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
//	objects.add(make_shared<xy_rect>(10, 5, 1, 10, -2, make_shared<lambertian>(color(0.5))));
//	// objects.add(make_shared<sphere>(point3(0,5,0), 2, difflight));
//
//
//	return objects;
//}

hittable_list cornell_box();

//hittable_list volumeTest() {
//	hittable_list objects;
//
//	objects.add(make_shared<sphere>(point3(0, 0, 0), 3.0f, make_shared<volume>(color(0.8f, 0.1f, 0.1f))));
//	objects.add(make_shared<xy_rect>(0, 1000, 0, 1000, 10, make_shared<lambertian>(color(0.5))));
//	return hittable_list(objects);
//}
#endif // !RAY_EXPLEAM_H
