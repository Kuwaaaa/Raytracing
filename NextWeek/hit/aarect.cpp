#include "aarect.h"

xy_rect::xy_rect(double _x0, double _x1, double _y0, double _y1, double _k,
	shared_ptr<material> mat, const std::string_view& id)
	: x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat), hittable(id) {
	if (x0 > x1) std::swap(x0, x1);
	if (y0 > y1) std::swap(y0, y1);
};


bool xy_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	auto t = (k - r.origin().z()) / r.direction().z();
	if (t < t_min || t > t_max)
		return false;
	auto x = r.origin().x() + t * r.direction().x();
	auto y = r.origin().y() + t * r.direction().y();
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	auto outward_normal = vec3(0, 0, 1);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	rec.hittedObject = entity.id;

	return true;
}


bool xy_rect::bounding_box(double time0, double time1, aabb& output_box) const
{
	// The bounding box must have non-zero width in each dimension, so pad the Z
			// dimension a small amount.
	output_box = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
	return true;
}

//const vec3& xy_rect::random(const point3& origin) const
//{
//	auto random_point = point3(random_double(x1, x0), random_double(y1, y0), k);
//	return random_point - origin;
//}


bool xz_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	auto t = (k - r.origin().y()) / r.direction().y();
	if (t < t_min || t > t_max)
		return false;
	auto x = r.origin().x() + t * r.direction().x();
	auto z = r.origin().z() + t * r.direction().z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	auto outward_normal = vec3(0, 1, 0);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	rec.hittedObject = entity.id;

	return true;
}

xz_rect::xz_rect(double _x0, double _x1, double _z0, double _z1, double _k,
	shared_ptr<material> mat, const std::string_view& id)
	: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat), hittable(id)
{
	
};


bool xz_rect::bounding_box(double time0, double time1, aabb& output_box) const
{
	// The bounding box must have non-zero width in each dimension, so pad the Y
	// dimension a small amount.
	output_box = aabb(point3(x0, k - 0.0001, z0), point3(x1, k + 0.0001, z1));
	return true;
}

inline const double xz_rect::pdf_value(const point3& origin, const vec3& v) const
{
	hit_record rec;
	if (!this->hit(ray(origin, v), 0.001, infinity, rec))
		return 0;

	auto area = (x1 - x0) * (z1 - z0);
	auto distance_squared = rec.t * rec.t * v.length_squared();
	//auto cosine = fabs(dot(v, rec.normal) / v.length());

	//return distance_squared / (cosine * area);
	return distance_squared / area;
}

inline const vec3 xz_rect::random(const point3& origin) const
{
	auto random_point = point3(random_double(x1, x0), k, random_double(z1, z0));
	return random_point - origin;
}

yz_rect::yz_rect(double _y0, double _y1, double _z0, double _z1, double _k,
	shared_ptr<material> mat, const std::string_view& id)
	: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat), hittable(id)
{

}


bool yz_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	auto t = (k - r.origin().x()) / r.direction().x();
	if (t < t_min || t > t_max)
		return false;
	auto y = r.origin().y() + t * r.direction().y();
	auto z = r.origin().z() + t * r.direction().z();
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	auto outward_normal = vec3(1, 0, 0);
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);
	rec.hittedObject = entity.id;

	return true;
}

bool yz_rect::bounding_box(double time0, double time1, aabb& output_box) const
{
	// The bounding box must have non-zero width in each dimension, so pad the X
			// dimension a small amount.
	output_box = aabb(point3(k - 0.0001, y0, z0), point3(k + 0.0001, y1, z1));
	return true;
}

//const vec3& yz_rect::random(const point3& origin) const
//{
//	auto random_point3 = point3(k, random_double(y0, y1), random_double(z0, z1));
//	return random_point3 - origin;
//}
