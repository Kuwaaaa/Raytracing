#pragma once

#ifndef ONB_H
#define ONB_H

#include "vec3/vec3.h"

class Onb {
public:
	Onb() = default;

    inline vec3 operator[](int i) const;
    vec3 u() const;
    vec3 v() const;
    vec3 w() const;
    vec3 local(double a, double b, double c) const;
    vec3 local(const vec3& a) const;
    void build_from_w(const vec3&);

public:
    vec3 axis[3];
};


#endif // !ONB_H
