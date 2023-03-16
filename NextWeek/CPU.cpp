#include "CPU.h"

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

	if (!m_scene.world.hit(r, 0.001, infinity, hrec))
		return m_scene.background;

	ray scattered;
	color emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
	if (!hrec.mat_ptr->scatter(r, hrec, srec))
		return emitted;

	auto lightPdf = make_shared<hittable_pdf>(m_scene.lights, hrec.p);
	auto cosPdf = make_shared<cosine_pdf>(hrec.normal);
	mixture_pdf mixed_pdf(cosPdf, cosPdf);
	//mixture_pdf mixed_pdf(lightPdf, lightPdf);

	scattered = ray(hrec.p, mixed_pdf.generate(), r.time());
	auto pdfValue = mixed_pdf.value(scattered.direction());

	//return (emitted + attenuation * ray_color(scattered, depth - 1)) / lifePR;
	return emitted + srec.attenuation * hrec.mat_ptr->scattering_pdf(r, hrec, scattered)
		* ray_color(scattered, depth - 1) / pdfValue;
}


void CPU::reset_Image()
{
	for (int i = 0; i < m_scene.image_size; i++)
		m_scene.m_sum_ImageBuffer[i] = 0;
	ssp_sum = 0;
}


void CPU::setMaxDepth(int depth)
{
	m_scene.max_depth = depth;
}

CPU::CPU(Scene scene)
	:Device(scene, scene.getCamera()) {
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
	double r, g, b;
	ssp_sum += ssp;
	auto scale = 1.0 / ssp_sum;

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
