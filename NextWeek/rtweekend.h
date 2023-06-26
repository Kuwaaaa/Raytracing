#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>      // 获得机器相关的数据类型大小上限
#include <memory>
//#include <cstdlib>
#include <random>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Function

// 角度转弧度
inline 
double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// Returns a random real in [0, 1]
inline 
double random_double() {
    // I dont know why rand() is not working in windows.
#ifndef _WIN32
    return rand() / (RAND_MAX + 1.0);
#else
    // Statement variable with thread_local to provide standalone random engine to every thread.
    static thread_local std::default_random_engine e;
    static thread_local std::uniform_real_distribution<double> u(0.0, 1.0);
    return u(e);
#endif // !WIN32
}

inline 
double random_double(double min, double max) {
    // Returns a random real in [min, max]
    return min + (max - min)*random_double(); 
} 

inline 
double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}

#endif // RTWEEKEND_H
