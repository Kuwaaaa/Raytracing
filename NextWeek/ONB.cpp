#include "ONB.h"

inline vec3 Onb::operator[](int i) const
{
    return axis[i];
}

vec3 Onb::u() const
{
    return axis[0];
}

vec3 Onb::v() const
{
    return axis[1];
}

vec3 Onb::w() const
{
    return axis[2];
}

vec3 Onb::local(double a, double b, double c) const
{
    return a * u() + b * v() + c * w();
}

vec3 Onb::local(const vec3& a) const
{
    return a.x() * u() + a.y() * v() + a.z() * w();
}

void Onb::build_from_w(const vec3& n)
{
    axis[2] = unit_vector(n);
    // Ensure this vec is not parallel to normal. 
    vec3 a = (fabs(w().x()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
    axis[1] = unit_vector(cross(w(), a));
    axis[0] = cross(w(), v());
}
