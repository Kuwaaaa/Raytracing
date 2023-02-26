#ifndef RAY_H
#define RAY_H

class ray {
    public:
        ray() {}
        ray(const point3& origin, const vec3& direction, double time = 0.0)
            : orig(origin), dir(direction), tm(time)
            { }

        // With const and &, it can be speed up a lot. Here test in random scene at expleam
        // debug: 8.9s, relese: 1.43s
        /*point3 origin() const { return orig; }
        vec3 direction() const { return dir; }*/

        // debug 7.89s, relese: 1.33s
        const point3& origin() const { return orig; };
        const vec3& direction() const { return dir; };
        double time() const { return tm;}


        point3 at(double t) const {
            return orig + t*dir;
        }

    public:
        point3 orig;
        vec3 dir;
        double tm;
};

#endif // RAY_H
