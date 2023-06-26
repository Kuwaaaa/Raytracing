#pragma once

#include "Device.h"
#include "rtweekend.h"
#include "vec3/vec3.h"
#include "vec3/ray.h"
#include "color.h"
#include "scene/scene.h"
#include "hit/bvh.h"
#include "pdf.h"
#include "avgTest.h"

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
	CPU(Scene scene, bool par = true);
	virtual void render(int) override;

	void setGlTex(GLuint texId);
	void reset_Image();
	void setMaxDepth(int depth);
	const Scene& get_scene() const;
	void setPar(bool);

private:
	color PerPixel(size_t x, size_t y, int ssp);
	color ray_color(const ray& r, int depth);

private:
	std::vector<std::size_t> m_ImageVerticalIter, m_ImageHorizontalIter;
	bool isPar;
	Accumulator acc1;
	Accumulator acc2;
};




