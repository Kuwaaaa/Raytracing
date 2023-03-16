#ifndef HITTABLE_LIST
#define HITTABLE_LIST

#include "hittable.h"
#include "aabb.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
public:
	hittable_list() = default;
	hittable_list(std::shared_ptr<hittable> object);

	void clear();
	void add(std::shared_ptr<hittable> object);
	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(
		double time0, double time1, aabb& output_box) const override;

public:
	std::vector<std::shared_ptr<hittable>> objects;
};



#endif // HITTABLE_LIST
