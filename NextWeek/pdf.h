#pragma once
#ifndef PDF_H
#define PDF_H

#include "vec3/vec3.h"
#include "hit/hittable.h"
#include "ONB.h"

const vec3 random_cosine_direction();

class pdf {
public:
	~pdf() = default;

    // normalize 需要除去的系数
	virtual const double value(const vec3& direction) const = 0;   
	virtual const vec3 generate() const = 0;
};


class cosine_pdf : public pdf {
public: 
    cosine_pdf(const vec3&);
    
    virtual const double value(const vec3&) const override;
    virtual const vec3 generate() const;

private:
    Onb uvw;
};

class hittable_pdf : public pdf {
public:
    hittable_pdf(shared_ptr<hittable> p, const point3& origin);

    virtual const double value(const vec3& direction) const override;
    virtual const vec3 generate() const override;

public:
    point3 o;
    shared_ptr<hittable> ptr;
};

class mixture_pdf : public pdf {
public:
    mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1);

    virtual const double value(const vec3& direction) const override;
    virtual const vec3 generate() const override;

    ~mixture_pdf();

public:
    shared_ptr<pdf> p[2];
};

#endif // !PDF_H
