#include "ray_expleam.h"

hittable_list cornell_box()
{
	hittable_list objects;

	auto red = make_shared<lambertian>(color(.65, .05, .05));
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	auto green = make_shared<lambertian>(color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(color(15, 15, 15));

	// Left plane
	objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green, "left plane")));
	// Right plane
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red, "right plane"));
	// Top light
	objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light, "light")));
	// Bottom plane
	objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 0, white, "bottom plane")));
	// Top plane
	objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white, "top plane")));
	// Back plane
	objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white, "back plane")));

	// Cuboid
	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white, "box1");
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	objects.add(box1);
	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white, "box2");
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	objects.add(box2);

	//objects.add(make_shared<sphere>(point3(0, 1, 0), 500, red));

	return objects;
}
