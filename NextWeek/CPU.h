#pragma once

#include "Device.h"
#include "rtweekend.h"
#include "color.h"
#include "scene/scene.h"
#include "hit/hittable_list.h"
#include "material/material.h"
#include "hit/bvh.h"
#include "util/timer.h"

#include <algorithm>
#include <execution>

#include <glad/glad.h>

#include <vector>


class CPU :
	public Device
{
public:
	CPU(Scene scene);
	virtual void render(int ssp = 50) override;
	const Scene& get_scene() const;
	void setGlTex(GLuint texId);

private:
	color PerPixel(size_t x, size_t y, int ssp);
	color ray_color(const ray& r, int depth);

private:
	Scene m_scene;
	Camera m_camera;
	std::vector<std::size_t> m_ImageVerticalIter, m_ImageHorizontalIter;
};

color CPU::PerPixel(size_t x, size_t y, int ssp)
{
	color pixel_color(0, 0, 0);
	double u, v;
	for (int s = 0; s < ssp; ++s) {
		u = (x + random_double()) / (m_scene.image_width - 1);
		v = (y + random_double()) / (m_scene.image_height - 1);
		ray r = m_scene.cam.get_ray(u, v);
		pixel_color += ray_color(r, m_scene.max_depth);
	}

	return pixel_color;
}


color CPU::ray_color(const ray& r, int depth)
{
	hit_record rec;

	double lifePR = 0.8;

	if (random_double() > lifePR)
		return color(0, 0, 0);

	/*if (depth <= 0)
		return color(0, 0, 0);*/

	if (!m_scene.world.hit(r, 0.001, infinity, rec))
		return m_scene.background;

	ray scattered;
	color attenuation;
	color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return (emitted + attenuation * ray_color(scattered, depth - 1)) / lifePR;
	//return emitted + attenuation * ray_color(scattered, depth - 1);
}


CPU::CPU(Scene scene)
	:m_scene(scene), m_camera(scene.getCamera()) {
	m_ImageVerticalIter.resize(scene.image_width);
	m_ImageHorizontalIter.resize(scene.image_height);
	for (int i = 0; i < scene.image_width; i++)
		m_ImageVerticalIter[i] = i;
	for (int i = 0; i < scene.image_height; i++)
		m_ImageHorizontalIter[i] = i;

}
void CPU::render(int ssp /*= 50*/)
{
	double r, g, b;
	static double ssp_sum = 0;
	ssp_sum += ssp;
	auto scale = 1.0 / ssp_sum;

	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(), [&](std::size_t x)
		{
			std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(), [&](std::size_t y)
				{
					color pixel_color(0, 0, 0);
					pixel_color = PerPixel(x, y, ssp);
					size_t idx = (x + y * m_scene.image_width) * 3;

					m_scene.m_sum_ImageBuffer[idx]	   += pixel_color[0];
					m_scene.m_sum_ImageBuffer[idx + 1] += pixel_color[1];
					m_scene.m_sum_ImageBuffer[idx + 2] += pixel_color[2];

					
					r = sqrt(scale * m_scene.m_sum_ImageBuffer[idx]);
					g = sqrt(scale * m_scene.m_sum_ImageBuffer[idx + 1]);
					b = sqrt(scale * m_scene.m_sum_ImageBuffer[idx + 2]);

					m_scene.m_ImageBuffer[idx]	   = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
					m_scene.m_ImageBuffer[idx + 1] = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
					m_scene.m_ImageBuffer[idx + 2] = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
				});
		});
}

const Scene& CPU::get_scene() const
{
	return m_scene;
}

void CPU::setGlTex(GLuint texId)
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_scene.image_width, m_scene.image_height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, m_scene.m_ImageBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

}
