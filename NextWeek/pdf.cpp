#include "pdf.h"

cosine_pdf::cosine_pdf(const vec3& n)
{
	uvw.build_from_w(n);
}


const double cosine_pdf::value(const vec3& direction) const
{
	auto cosine = dot(unit_vector(direction), uvw.w());
	return (cosine < 0) ? 0 : cosine / pi;
}


const vec3 cosine_pdf::generate() const
{
	return uvw.local(random_cosine_direction());
}

const vec3 random_cosine_direction()
{
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = sqrt(1 - r2);

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);

    return vec3(x, y, z);
}


hittable_pdf::hittable_pdf(hittable* p, const point3& origin)
    : ptr(p), o(origin) { }


const double hittable_pdf::value(const vec3& direction) const
{
    return ptr->pdf_value(o, direction);
}


const vec3 hittable_pdf::generate() const
{
    return ptr->random(o);
}


mixture_pdf::mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1)
{
    p[0] = p0;
    p[1] = p1;
}


const double mixture_pdf::value(const vec3& direction) const
{
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
}


const vec3 mixture_pdf::generate() const
{
    if (random_double() < 0.5)
        return p[0]->generate();
    else
        return p[1]->generate();
}


mixture_pdf::~mixture_pdf()
{
    
}
