#include "scene.h"

Scene::Scene(bvh_node w, Camera c, int spp, color bg, int imgW, float ar, int max_d)
	: world(w), cam(c), samples_per_pixel(spp), background(bg),
	image_width(imgW), aspect_radio(ar), max_depth(max_d)
{
	initImageBuffer();
}


Scene::Scene(int spp, color bg, int imgW, float ar, int max_d, SCENE_EXPLEAM exp)
	: samples_per_pixel(spp), background(bg), image_width(imgW), aspect_radio(ar), max_depth(max_d)
{
	double aspect_ratio = 16 / 9;
	point3 lookfrom = point3(13, 2, 13);
	point3 lookat = point3(0, 0, 0);
	point3 vup = point3(0, 1, 0);
	double vfov = 40.0;        // vertical field-of-view in degrees
	double aperture = 0;
	double focus_dist = (point3(13, 2, 13) - point3(0, 0, 0)).length();

	switch (exp)
	{
		/*case SCENE_EXPLEAM::RANDOM:
			world = random_scene();
			aperture = 0.1f;
			break;

		default:
		case SCENE_EXPLEAM::TWO_SPHERES:
			world = two_spheres();
			break;

		case SCENE_EXPLEAM::TWO_PERLIN_SPHERES:
			world = two_perlin_spheres();
			break;

		case SCENE_EXPLEAM::TEXTURE:
			vfov = 20;
			world = earth();
			break;

		case SCENE_EXPLEAM::SIMPLE_LIGHT:
			world = simple_light();
			samples_per_pixel = 400;
			lookfrom = point3(26, 3, 6);
			lookat = point3(0, 2, 0);
			vfov = 20.0;
			break;*/

	case SCENE_EXPLEAM::CORNELL_BOX:
		world = cornell_box();
		lights = make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>());
		image_width = 640;
		aspect_radio = 1.0f;
		samples_per_pixel = 400;
		background = color(0, 0, 0);
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;

		/*case SCENE_EXPLEAM::VOLUMETEST:
			world = volumeTest();
			break;*/
	}

	Camera t_cam(aspect_radio, lookfrom, lookat, vup, vfov, aperture, focus_dist);
	this->cam = t_cam;

	initImageBuffer();
}

void Scene::initImageBuffer()
{
	image_height = static_cast<int>(image_width / aspect_radio);
	image_size = image_width * image_height * 3;
	m_ImageBuffer = new unsigned char[image_size](0);
	m_sum_ImageBuffer = new unsigned char[image_size](0);
}
