#include "box.h"

box::box(const point3& p0, const point3& p1, shared_ptr<material> ptr, const std::string_view& id)
    :hittable(id)
{
    box_min = p0;
    box_max = p1;

    sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr, "1"));
    sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr, "2"));

    sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr, "3"));
    sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr, "4"));

    sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr, "5"));
    sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr, "6"));

}


bool box::hit(const ray& ray, double t_min, double t_max, hit_record& rec) const {
    return sides.hit(ray, t_min, t_max, rec);
}

bool box::bounding_box(double time0, double time1, aabb& output_box) const
{
    output_box = aabb(box_min, box_max);
    return true;
}
