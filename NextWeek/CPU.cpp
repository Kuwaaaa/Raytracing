#include "CPU.h"
#include <stdlib.h>

color CPU::PerPixel(size_t x, size_t y, int ssp)
{
	color pixel_color(0, 0, 0);
	double u, v;
	for (int s = 0; s < ssp; ++s) {
		u = (x + random_double()) / (m_scene.image_width - 1);
		v = (y + random_double()) / (m_scene.image_height - 1);
		ray r = m_scene.cam.get_ray(u, v);
		pixel_color += de_nan(ray_color(r, m_scene.max_depth));
	}

	return pixel_color;
}


color CPU::ray_color(const ray& r, int depth)
{
	hit_record hrec;
	scatter_record srec;
	double lifePR = 0.8;

	/*if (random_double() > lifePR)
		return color(0, 0, 0);*/

	if (depth <= 0)
		return color(0, 0, 0);

	if (!m_scene.world.hit(r, 0.0001, infinity, hrec))
		return m_scene.background;

	ray scattered;
	color emitted = hrec.mat_ptr->emitted(r, hrec);
	if (!hrec.mat_ptr->scatter(r, hrec, srec))
		return emitted;

	static xz_rect* light = new xz_rect(213, 343, 227, 332, 554, make_shared<diffuse_light>(color(15, 15, 15)), "light");

	//hittable_pdf lightPdf(aaaaa, hrec.p);
	shared_ptr<hittable_pdf> lightPdf = make_shared<hittable_pdf>(light, hrec.p);
	shared_ptr<pdf> matPdf = srec.pdf_ptr;
	auto mixed_pdf = make_shared<mixture_pdf>(matPdf, matPdf);

	scattered = ray(hrec.p, mixed_pdf->generate(), r.time());
	color dirColor(0);
	vec3 toLight = lightPdf->generate();
	hit_record htemp;
	scatter_record stemp;
	bool hitted = m_scene.world.hit(ray(hrec.p, toLight.normal()), 0.0001, infinity, htemp);
	
	if (htemp.hittedObject == "light")
	{
		double lightNorDotToLight = (dot(-toLight.normal(), htemp.normal));
		double objToLight = (dot(hrec.normal, toLight.normal()));

		//htemp.mat_ptr->scatter(ray(hrec.p,toLight), htemp, stemp);
		auto value = lightPdf->value(toLight);
		color lc = light->mp->emitted(r, hrec);
		dirColor = lc * srec.fr * lightNorDotToLight * objToLight / value;
	}
	auto pdfValue = mixed_pdf->value(toLight);

	auto sPdf = hrec.mat_ptr->scattering_pdf(r, hrec, scattered);

	color boundColor(0);
	boundColor = srec.fr * ray_color(scattered, depth - 1);

	//return (emitted + attenuation * ray_color(scattered, depth - 1)) / lifePR;
	return dirColor + boundColor;
	//return emitted + dirColor + srec.attenuation * sPdf *color;
}


void CPU::reset_Image()
{
	memset(m_scene.m_sum_ImageBuffer, 0, m_scene.image_size * sizeof(double));
	ssp_sum = 0;
}


void CPU::setMaxDepth(int depth)
{
	m_scene.max_depth = depth;
}


CPU::CPU(Scene scene, bool par/*=true*/)
	:Device(scene, scene.getCamera()), isPar(par) 
{
	m_type = DEVICE_TYPE::DEVICE_CPU;
	m_ImageVerticalIter.resize(scene.image_width);
	m_ImageHorizontalIter.resize(scene.image_height);
	for (int i = 0; i < scene.image_width; i++)
		m_ImageVerticalIter[i] = i;
	for (int i = 0; i < scene.image_height; i++)
		m_ImageHorizontalIter[i] = i;
}


void CPU::render(int ssp /*= 50*/)
{
	double r{ 0 }, g{ 0 }, b{ 0 };
	ssp_sum += ssp;
	auto scale = 1.0 / ssp_sum;

	if (isPar)
	{
		std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(), [&](std::size_t x)
			{
				std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(), [&](std::size_t y)
					{
						color pixel_color(0, 0, 0);
						pixel_color = PerPixel(x, y, ssp);
						size_t idx = (x + y * m_scene.image_width) * 3;

						m_scene.m_sum_ImageBuffer[idx] += pixel_color[0];
						m_scene.m_sum_ImageBuffer[idx + 1] += pixel_color[1];
						m_scene.m_sum_ImageBuffer[idx + 2] += pixel_color[2];

						r = sqrt(scale * m_scene.m_sum_ImageBuffer[idx]);
						g = sqrt(scale * m_scene.m_sum_ImageBuffer[idx + 1]);
						b = sqrt(scale * m_scene.m_sum_ImageBuffer[idx + 2]);

						m_scene.m_ImageBuffer[idx] = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
						m_scene.m_ImageBuffer[idx + 1] = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
						m_scene.m_ImageBuffer[idx + 2] = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
					});
			});
	}
	else
	{
		// For debug
		for(int i=0; i<m_scene.image_height; i++)
			for (int j = 0; j < m_scene.image_width; j++)
			{
				color pixel_color(0, 0, 0);
				pixel_color = PerPixel(j, i, ssp);
				size_t idx = (j + i * m_scene.image_width) * 3;

				m_scene.m_sum_ImageBuffer[idx] += pixel_color[0];
				m_scene.m_sum_ImageBuffer[idx + 1] += pixel_color[1];
				m_scene.m_sum_ImageBuffer[idx + 2] += pixel_color[2];

				r = sqrt(scale * m_scene.m_sum_ImageBuffer[idx]);
				g = sqrt(scale * m_scene.m_sum_ImageBuffer[idx + 1]);
				b = sqrt(scale * m_scene.m_sum_ImageBuffer[idx + 2]);

				m_scene.m_ImageBuffer[idx] = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
				m_scene.m_ImageBuffer[idx + 1] = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
				m_scene.m_ImageBuffer[idx + 2] = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
			}
	}

}


const Scene& CPU::get_scene() const
{
	return m_scene;
}


void CPU::setPar(bool par)
{
	isPar = par;
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
