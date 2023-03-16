#pragma once

#include "Device.h"
#include "rtweekend.h"
#include "vec3/vec3.h"
#include "vec3/ray.h"
#include "color.h"
#include "scene/scene.h"
#include "hit/bvh.h"
#include "pdf.h"

// Multithread perpixel
#include <algorithm>
#include <execution>

#include <glad/glad.h>

#include <float.h>
#include <vector>


class CPU :
	public Device
{
public:
	CPU(Scene scene);
	virtual void render(int ssp = 50) override;
	const Scene& get_scene() const;
	void setGlTex(GLuint texId);
	void reset_Image();
	void setMaxDepth(int depth);

private:
	color PerPixel(size_t x, size_t y, int ssp);
	color ray_color(const ray& r, int depth);

private:
	std::vector<std::size_t> m_ImageVerticalIter, m_ImageHorizontalIter;
};




