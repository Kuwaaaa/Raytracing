#ifndef SCENE_H
#define SCENE_H

#include "../camera.h"
#include "../hit/bvh.h"
#include "../ray_expleam.h"
#include "../vec3/vec3.h"

#include <vector>

enum class SCENE_EXPLEAM {
	RANDOM,
	TWO_SPHERES,
	TWO_PERLIN_SPHERES,
	TEXTURE,
	SIMPLE_LIGHT,
	CORNELL_BOX,
	VOLUMETEST
};

class Scene {
public:
	Scene(bvh_node w, Camera c, int spp, color bg, int imgW, float ar, int max_d);
	Camera& getCamera() { return cam; }
	Scene(int spp, color bg, int imgW, float ar, int max_d, SCENE_EXPLEAM exp);

private:
	void initImageBuffer();

public:
	bvh_node world;
	Camera cam;
	color background;
	shared_ptr<hittable> lights;
	int samples_per_pixel;
	int image_width;
	int image_height;
	int image_size;
	int max_depth;
	float aspect_radio;
	// Image result buffer
	unsigned char* m_ImageBuffer;
	double* m_sum_ImageBuffer;
};

#endif // !SCENE_H
